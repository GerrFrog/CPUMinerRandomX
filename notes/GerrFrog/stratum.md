# Stratum communication
## Stratum V2
### Server sending
#### **mining.notify**
Expected
```JSON
{
    "method": "mining.notify", 
    "params": [
        "Challenge", 
        "Target,"
        "Difficulty,"
        "PoolAddress"
    ]
}
```

Real for aionpool
```JSON
{
    "id": null,
    "jsonrpc": "2.0",
    "method": "mining.notify",
    "params": [
        "000120c9", // Job ID
        true, // Clean job
        "00000513c64a545275c11b80ab9d655e642736302e9c28b71b577f41629a9774", // Job target
        "ff62758c06cfbbe3ec0164eaa64cbc1503b8d90dae727171f3ab89750b7dc8b6" // Header hash
    ]
}
```

#### **mining.set_target**
Expected
```JSON
{
    "params": [
        "NEW DIFFICULTY"
    ]
}
```

Real for aionpool
```JSON
{
    "id": null,
    "jsonrpc": "2.0",
    "method": "mining.set_target",
    "params": [
        "0020000000000000000000000000000000000000000000000000000000000000" // New difficulty
    ]
}
```



