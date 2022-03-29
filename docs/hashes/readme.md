# Content table
- [Content table](#content-table)
- [Hashes namespace](#hashes-namespace)
  - [Example](#example)
# Hashes namespace
This namespace contains all classes to hash with different algorithm

- [x] **Hashes::SHA_256** Implement function of hash sha-256. Contains two methods
    - Get pointer the first element array by hash
    - Fill a input buffer by hash


## Example

Use SHA256 algorithm
```C
int main()
{
    Hashes::SHA_256 Hash;
    char hash_message[] = "Hello world";
  
    for (int i = 0; i < 65; i++) {
        std::cout << Hash.get_hash_sha256_array(hash_message, strlen(hash_message))[i];
    }
    char buffer[65] = { 0 };
        Hash.completion_hash_sha256_array(hash_message, strlen(hash_message), buffer);
    std::cout <<"\n"<<std::endl;
    for (int i = 0; i < 65; i++) {
        std::cout << buffer[i];
    }
    std::cout << "\n";
}
```