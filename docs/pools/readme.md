# Content table
- [Content table](#content-table)
- [Pools namespace](#pools-namespace)
  - [Pools::Implementors](#poolsimplementors)
    - [Pools::Implementors::Parsers](#poolsimplementorsparsers)
      - [Pools::Implementors::Parsers::V1](#poolsimplementorsparsersv1)
      - [Pools::Implementors::Parsers::V2](#poolsimplementorsparsersv2)
  - [Pools::Stratum_V1](#poolsstratum_v1)
  - [Pools::Stratum_V2](#poolsstratum_v2)
- [Examples](#examples)
# Pools namespace
This namespace contains all mining pools objects to connect to them.

## Pools::Implementors
This namespace contains classes for pool object implementation

- [x] **Pools::Implementors::Stratum_Socket** This class is used for connection with pool using stratum socket protocol

### Pools::Implementors::Parsers
This namespace contains classes for parsing responses from pool (server)

#### Pools::Implementors::Parsers::V1
This namespace contains classes for parsing stratum protocol response using Version 1

- [x] **Pools::Implementors::Parsers::V1::XMR_Parser** This class is used to parse responses from XMR pools
- [ ] **Pools::Implementors::Parsers::V1::ETH_Parser** This class is used to parse responses from ETH pools

#### Pools::Implementors::Parsers::V2
This namespace contains classes for parsing stratum protocol response using Version 2

- [ ] **Pools::Implementors::Parsers::V2::Standard_Parser** This class is used to parse responses from any pool which is using standard methods of stratum

## Pools::Stratum_V1
This namespace contains classes to communicate with pool using Stratum Version 1

- [x] **Pools::Stratum_V1::XMR_Pool** This class is used to communicate with XMR pool
- [ ] **Pools::Stratum_V1::ETH_Pool** This class is used to communicate with ETH pool

## Pools::Stratum_V2
This namespace contains classes to communicate with pool using Stratum Version 2

- [ ] **Pools::Stratum_V2::Pool** This class is used to communicate with any pool

# Examples
**NOTE:** If example does not work check the notes/GerrFrog/pools.md file for certain pool configuration.

Connector using Global_Pool object
```C
nlohmann::json configuration = {
    {"host", "xmr.2miners.com"},
    {"port", "2222"},
    {"login", "48edfHu7V9Z84YzzMa6fUueoELZ9ZRXq9VetWzYGzKt52XU5xvqgzYnDK9URnRoJMk1j8nLwEVsaSWJ4fhdUyZijBGUicoD"},
    {"password", "x"}
};

Pools::Stratum_V2::Pool pool(configuration);
std::cin.ignore();
```



