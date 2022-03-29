# Content table
- [Content table](#content-table)
- [NTP Servers](#ntp-servers)
- [Stratum protocol](#stratum-protocol)
  - [Mining pool](#mining-pool)
    - [How a Mining Pool Works](#how-a-mining-pool-works)
  - [Stratum + TCP](#stratum--tcp)
  - [Example of using](#example-of-using)
    - [**Subscribe to mining pool**](#subscribe-to-mining-pool)
    - [**Authorize**](#authorize)
- [Literature](#literature)
# NTP Servers
NTP is a network protocol for building a computer clock using latency networks.

NTP, based on the Marzullo algorithm, uses the UDP protocol and transmission time to operate. The NTP system is robust to changes in media latency. In version 4, the result is 10 ms (1/100 s) when working over the Internet, and up to 0.2 ms (1/5000 s) and better inside the resulting networks.

# Stratum protocol
Stratum V2 is the next generation protocol for pooled mining. It focuses on making data transfers more efficient, reducing physical infrastructure requirements for mining operations, and increasing security. Additionally, Stratum V2 introduces three new sub-protocols that allow miners to select their own transaction sets through a negotiation process with pools, improving decentralization.
## Mining pool
A mining pool is a joint group of cryptocurrency miners who combine their computational resources over a network to strengthen the probability of finding a block or otherwise successfully mining for cryptocurrency. 

### How a Mining Pool Works
Individually, participants in a mining pool contribute their processing power toward the effort of finding a block. If the pool is successful in these efforts, they receive a reward, typically in the form of the associated cryptocurrency.

The stratum overlay protocol was extended to support pooled mining as a replacement for obsolete getwork protocol.

Rewards are usually divided between the individuals who contributed, according to the proportion of each individual's processing power or work relative to the whole group. In some cases, individual miners must show proof of work in order to receive their rewards. 
 
## Stratum + TCP
To connecto to pool the miner uses Stratum protocol with TCP connection. For example: **stratum+tcp//eu.luckpool.net/**

Stratum protocol uses JSON format with Sockets exchanges of data. TCP protocol provides that data will be received and sent.

## Example of using
### **Subscribe to mining pool**
Request
```JSON
{
    "id": 1,
    "method": "mining.subscribe",
    "params": [
        "YOU WALLET",
        "PASSWORD FROM POOL",
        "MINER NAME",
        "EXTRANONCE"
    ]
}
```

Response Success
```JSON
{
    "result": true
}
```

Response Failure
```JSON
{
    "error": [
        "Error number",
        "Error message"
    ]
}
```

### **Authorize**
Request
```JSON
{
    "id": 1,
    "method": "mining.authorize",
    "params": [
        "YOU WALLET",
        "PASSWORD FROM POOL"
    ]
}
```

Response Success
```JSON
{
    "result": true
}
```

Response Failure
```JSON
{
    "error": [
        "Error number",
        "Error message"
    ]
}
```

# Literature
- [Official Stratum documentation](https://ru.braiins.com/stratum-v2)
- [Stratum methods](https://en.bitcoin.it/wiki/Stratum_mining_protocol#mining.authorize)
- [Stratum RPC Protocol implementation in Grin](https://docs.grin.mw/wiki/api/stratum-rpc/)
- [Ethereum mining program Ethminer](https://programmer.ink/think/ethereum-mining-program-ethminer.html)
- [Stratum Specification](https://mvis.ca/stratum-spec.html)
- [Stratum mining protocol article](https://russianblogs.com/article/92581506828/)
- [Example of working with stratum and miner](https://braiins.com/stratum-v1/docs#example)
- [Stratum protocol methods](https://reference.cash/mining/stratum-protocol)
- [Stratum v1 methods for ETH](https://github.com/sammy007/open-ethereum-pool/blob/master/docs/STRATUM.md)
- [Stratum for XMR](https://github.com/jtgrassie/monero-pool/blob/master/sss.md)




