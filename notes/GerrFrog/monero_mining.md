# Content table
- [Content table](#content-table)
- [Monero mining algorithm](#monero-mining-algorithm)
  - [Step 1](#step-1)
  - [Step 2](#step-2)
  - [Step 3](#step-3)
  - [Step 4](#step-4)
  - [Step 5](#step-5)
- [Python pseudocode of algorithm](#python-pseudocode-of-algorithm)
  - [Overview](#overview)
  - [Hashing](#hashing)
- [XMRIG reverse](#xmrig-reverse)
- [Literature](#literature)
# Monero mining algorithm
## Step 1

A data structure called the Cache is created using argon2d with the input key K. Argon2d was originally designed as a memory-hard password hashing function. Computers generally have large pools of fast memory available to them, but memory is expensive on an ASIC. Requiring large amounts of memory is one of the most common defenses against specialized hardware. Argon2 uses a variety of techniques to ensure that a large (configurable) quantity of memory is used and that any time/memory tradeoff attacks are ineffective.

## Step 2

The Dataset (a read-only memory structure) is expanded from the Cache. Datasets are designed to be a large segment of memory that contain data the virtual machine will read. There are two values that control the size of the Dataset (RANDOMX_DATASET_BASE_SIZE and RANDOMX_DATASET_EXTRA_SIZE). Together, they place an upper bound on the total memory the algorithm requires. Extra size is used to push the memory slightly beyond a power of two boundary, which makes life harder for ASIC manufacturers. The actual Dataset generation is performed by loading data from the Cache, generating a set of SuperscalarHash instances, and then invoking those instances to get a final output. SuperscalarHash is designed to consume power while waiting for data from DRAM. This hurts an ASIC that attempts to compute Datasets dynamically from the Cache.

## Step 3

The Scratchpad (read/write memory) is initialized by performing a blake2 hash on the input data and using the resulting output to seed the AesGenerator. This generator uses AES-NI instructions to fill the Scratchpad. Generation of the initial Scratchpad uses AES transformations. This algorithm is already hardware-accelerated on modern CPUs, so an ASIC will gain no advantage implementing it. The Scratchpad itself is a (relatively) large read/write data structure designed specifically to fit in caches that are available in CPUs.

## Step 4

Now we get to the core of the algorithm: the randomized programs running on a virtual machine. The VM is executed by building a program using random bytes created using another generator. The RandomX virtual machine architecture is carefully designed to allow any sequence of 8-byte words to be a valid instruction. These instructions are designed to:

- Require double precision floating point operations
- Use 128-bit vector math
- Use all four IEEE 754 floating point rounding modes
- Read and write to the Scratchpad, which as previously stated is designed to fit entirely in CPU caches and thus be very fast
- Take advantage of branch prediction via a low probability branch instruction
- Execute instructions using the superscalar and out-of-order execution capabilities of a CPU

Each of these properties is a particular strength of general-purpose CPUs and requires additional die area to implement on an ASIC, reducing its advantage.

The resulting state of the VM after program execution is hashed and used to generate a new program. The number of loops executed in this fashion is configurable but is set to eight by default. This looping behavior was chosen to avoid situations where an ASIC miner might only implement a subset of possible operations and only run “easy” programs on the virtual machine. Since the subsequent program cannot be determined until the current one has been fully executed, a miner cannot predict whether the entirety of the chain will be “easy,” so it becomes impractical to implement a partial set of instructions.

## Step 5

Finally, the Scratchpad and Register File (the virtual machine’s registers) are hashed using AesHash followed by a final blake2 hash. This step offers no significant ASIC resistance beyond the use of AES instructions, but is included to show the final hashing to a 64-byte value.

# Python pseudocode of algorithm
## Overview
1. Connect to pool using socket and create queue of job (contains all job responses from pool)

```python
pool_ip = socket.gethostbyname(pool_host)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((pool_ip, pool_port))

q = Queue()
proc = Process(target=worker, args=(q, s))
proc.daemon = True
proc.start()
```

When new item is in q (Queue) it will call "worker" function.

2. Login to pool

```python
login = {
    'method': 'login',
    'params': {
        'login': wallet_address,
        'pass': pool_pass,
        'rigid': '',
        'agent': 'stratum-miner-py/0.1'
    },
    'id':1
}

s.sendall(str(json.dumps(login)+'\n').encode('utf-8'))
```

3. Now we activate listener for pool (asynchronous). It will get any responses from pool and push it to q (Queue).

```python
while 1:
    line = s.makefile().readline()
    r = json.loads(line)
    error = r.get('error')
    result = r.get('result')
    method = r.get('method')
    params = r.get('params')
    if error:
        print('Error: {}'.format(error))
        continue
    if result and result.get('status'):
        print('Status: {}'.format(result.get('status')))
    if result and result.get('job'):
        login_id = result.get('id')
        job = result.get('job')
        job['login_id'] = login_id
        q.put(job)
    elif method and method == 'job' and len(login_id):
        q.put(params)
```

There is a little parser for pool messages (if-conditions). If pool message contains "job" it will push job object to q (Queue).

4. We activate work function to resolve new job (asynchronous). It will be activated when q (Queue) gets new job object.

```python
while 1:
    job = q.get()
    if job.get('login_id'):
        login_id = job.get('login_id')
        print('Login ID: {}'.format(login_id))
    blob = job.get('blob')
    target = job.get('target')
    job_id = job.get('job_id')
    height = job.get('height')
    block_major = int(blob[:2], 16)
    cnv = 0
    if block_major >= 7:
        cnv = block_major - 6
    if cnv > 5:
        seed_hash = binascii.unhexlify(job.get('seed_hash'))
        print('New job with target: {}, RandomX, height: {}'.format(target, height))
    else:
        print('New job with target: {}, CNv{}, height: {}'.format(target, cnv, height))
    target = struct.unpack('I', binascii.unhexlify(target))[0]
    if target >> 32 == 0:
        target = int(0xFFFFFFFFFFFFFFFF / int(0xFFFFFFFF / target))
    nonce = 1

    while 1:
        bin = pack_nonce(blob, nonce)
        if cnv > 5:
            hash = pyrx.get_rx_hash(bin, seed_hash, height)
        else:
            hash = pycryptonight.cn_slow_hash(bin, cnv, 0, height)
        hash_count += 1
        sys.stdout.write('.')
        sys.stdout.flush()
        hex_hash = binascii.hexlify(hash).decode()
        r64 = struct.unpack('Q', hash[24:])[0]
        if r64 < target:
            elapsed = time.time() - started
            hr = int(hash_count / elapsed)
            print('{}Hashrate: {} H/s'.format(os.linesep, hr))
            if nicehash:
                nonce = struct.unpack('I', bin[39:43])[0]
            submit = {
                'method':'submit',
                'params': {
                    'id': login_id,
                    'job_id': job_id,
                    'nonce': binascii.hexlify(struct.pack('<I', nonce)).decode(),
                    'result': hex_hash
                },
                'id':1
            }
            print('Submitting hash: {}'.format(hex_hash))
            s.sendall(str(json.dumps(submit)+'\n').encode('utf-8'))
            select.select([s], [], [], 3)
            if not q.empty():
                break
        nonce += 1
```

## Hashing
1. Get parameters from pool message to start hashing

```python
blob = job.get('blob') # String
target = job.get('target') # String
job_id = job.get('job_id') # String
height = job.get('height') # String
```

2. Calculate block_major and cnv parameters. This parameters determine algorithm (RandomX or Cryptonight).

```python
# Get first two character and convert them to HEX number
# s = "123"
# a = s[:2] -> 12
# b = int(s[:2], 16) -> 18
block_major = int(blob[:2], 16)

if block_major >= 7:
    cnv = block_major - 6
if cnv > 5:
    # It returns the binary string that is represented by any hexadecimal string
    seed_hash = binascii.unhexlify(job.get('seed_hash'))
    print('New job with target: {}, RandomX, height: {}'.format(target, height))
else:
    print('New job with target: {}, CNv{}, height: {}'.format(target, cnv, height))
```

3. Prepare target

```python
# Unpack target object from binary to object
# https://tirinox.ru/python-struct/
# I - data presented ad unsigned int
target = struct.unpack('I', binascii.unhexlify(target))[0]

# Check for algorithm
if target >> 32 == 0: ## shift to right by 32 digits
    target = int(0xFFFFFFFFFFFFFFFF / int(0xFFFFFFFF / target))

nonce = 1
```

4. Start solve job

```python
# Concatenate blob and nonce
def pack_nonce(blob, nonce):
    # Convert string to binary
    b = binascii.unhexlify(blob)
    # pack first 39 unsigned chars
    bin = struct.pack('39B', *bytearray(b[:39]))
    # Doen't matter
    if nicehash:
        bin += struct.pack('I', nonce & 0x00ffffff)[:3]
        bin += struct.pack('{}B'.format(len(b)-42), *bytearray(b[42:]))
    else:
        # concatenate bin with nonce
        bin += struct.pack('I', nonce)
        # format binary
        bin += struct.pack('{}B'.format(len(b)-43), *bytearray(b[43:]))
    return bin

bin = pack_nonce(blob, nonce)
# Check for algorithm
if cnv > 5:
    hash = pyrx.get_rx_hash(bin, seed_hash, height)
else:
    hash = pycryptonight.cn_slow_hash(bin, cnv, 0, height)
hash_count += 1 # increment hash count

sys.stdout.write('.') # as print
sys.stdout.flush()

# convert hash to HEX
hex_hash = binascii.hexlify(hash).decode()
r64 = struct.unpack('Q', hash[24:])[0]

# if we found nonce (submit work)
if r64 < target:
    # calculate hashrate
    elapsed = time.time() - started
    hr = int(hash_count / elapsed) 
    print('{}Hashrate: {} H/s'.format(os.linesep, hr))

    # Prepare message
    if nicehash:
        nonce = struct.unpack('I', bin[39:43])[0]
    submit = {
        'method':'submit',
        'params': {
            'id': login_id,
            'job_id': job_id,
            'nonce': binascii.hexlify(struct.pack('<I', nonce)).decode(),
            'result': hex_hash
        },
        'id':1
    }
    print('Submitting hash: {}'.format(hex_hash))

    # Send to pool
    s.sendall(str(json.dumps(submit)+'\n').encode('utf-8'))
    select.select([s], [], [], 3)
    if not q.empty():
        break

# else increment nonce
nonce += 1
```

# XMRIG reverse
1. Mining function src/backend/cpu/CpuWorker.cpp

```c
template<size_t N>
void xmrig::CpuWorker<N>::start()
{
    while (Nonce::sequence(Nonce::CPU) > 0) {
        if (Nonce::isPaused()) {
            do {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            while (Nonce::isPaused() && Nonce::sequence(Nonce::CPU) > 0);

            if (Nonce::sequence(Nonce::CPU) == 0) {
                break;
            }

            consumeJob();
        }

        bool first = true;
        alignas(16) uint64_t tempHash[8] = {};

        while (!Nonce::isOutdated(Nonce::CPU, m_job.sequence())) {
            const Job &job = m_job.currentJob();

            if (job.algorithm().l3() != m_algorithm.l3()) {
                break;
            }

            uint32_t current_job_nonces[N];
            for (size_t i = 0; i < N; ++i) {
                current_job_nonces[i] = readUnaligned(m_job.nonce(i));
            }

            bool valid = true;

            uint8_t miner_signature_saved[64];

            uint8_t* miner_signature_ptr = m_job.blob() + m_job.nonceOffset() + m_job.nonceSize();
            
            if (job.algorithm().family() == Algorithm::RANDOM_X) {
                if (first) {
                    first = false;
                    if (job.hasMinerSignature()) {
                        job.generateMinerSignature(m_job.blob(), job.size(), miner_signature_ptr);
                    }
                    randomx_calculate_hash_first(m_vm, tempHash, m_job.blob(), job.size());
                }

                if (!nextRound()) {
                    break;
                }

                if (job.hasMinerSignature()) {
                    memcpy(miner_signature_saved, miner_signature_ptr, sizeof(miner_signature_saved));
                    job.generateMinerSignature(m_job.blob(), job.size(), miner_signature_ptr);
                }
                randomx_calculate_hash_next(m_vm, tempHash, m_job.blob(), job.size(), m_hash);
            }

            if (valid) {
                for (size_t i = 0; i < N; ++i) {
                    const uint64_t value = *reinterpret_cast<uint64_t*>(m_hash + (i * 32) + 24);

                    if (value < job.target()) {
                        JobResults::submit(job, current_job_nonces[i], m_hash + (i * 32), job.hasMinerSignature() ? miner_signature_saved : nullptr);
                    }
                }
                m_count += N;
            }

            if (m_yield) {
                std::this_thread::yield();
            }
        }

        consumeJob();
    }
}
```

# Literature
1. [Monero mining algorithm overview](https://blog.trailofbits.com/2019/07/02/state/)
2. [Python monero miner](https://github.com/jtgrassie/monero-powpy)
3. [RandomX questions in reddit](https://www.reddit.com/r/MoneroMining/comments/low54m/random_question_about_rx_algorithm_and_hashing/)
4. [tevador community questions with code](https://kandi.openweaver.com/c++/tevador/RandomX)
5. [xmrig miner](https://github.com/xmrig/xmrig)




























