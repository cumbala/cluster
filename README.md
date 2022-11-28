# cluster
Implementation of cluster simplification algorithm for IZP course

## Building
`gcc`, `make` and `valgrind` are the only requirements. `Linux` is mandatory, because `valgrind` still does not have an ARM implementation and x86 version does not work on new ARM Macs even through Rosetta 2.
1. Clone this repo
2. `cd cluster`
3. `make`

### Valgrind
Valgrind is used to check for memory leaks. To run it, use `make valgrind`.

## Running
#### Usage: `./cluster <filename> [<size>]`

- `./cluster objekty` - Will print all points from `objekty` combined into one cluster

    Output:
    ```
    Clusters:
    cluster 0: 40[86,663] 43[747,938] 47[285,973] 49[548,422] 52[741,541] 56[44,854] 57[795,59] 61[267,375] 62[85,874] 66[125,211] 68[80,770] 72[277,272] 74[222,444] 75[28,603] 79[926,463] 83[603,68] 86[238,650] 87[149,304] 89[749,190] 93[944,835]
    ```
  
- `./cluster objekty 20` - Will print all points from `objekty` spread into 20 clusters

    Output:
    ```
    Clusters:
    cluster 0: 40[86,663]
    cluster 1: 43[747,938]
    cluster 2: 47[285,973]
    cluster 3: 49[548,422]
    cluster 4: 52[741,541]
    cluster 5: 56[44,854]
    cluster 6: 57[795,59]
    cluster 7: 61[267,375]
    cluster 8: 62[85,874]
    cluster 9: 66[125,211]
    cluster 10: 68[80,770]
    cluster 11: 72[277,272]
    cluster 12: 74[222,444]
    cluster 13: 75[28,603]
    cluster 14: 79[926,463]
    cluster 15: 83[603,68]
    cluster 16: 86[238,650]
    cluster 17: 87[149,304]
    cluster 18: 89[749,190]
    cluster 19: 93[944,835]
    ```
- `./cluster objekty 8` - Will print all points from `objekty` combined into 8 clusters
    
    Output:
    ```
    Clusters:
    cluster 0: 40[86,663] 56[44,854] 62[85,874] 68[80,770] 75[28,603] 86[238,650]
    cluster 1: 43[747,938]
    cluster 2: 47[285,973]
    cluster 3: 49[548,422]
    cluster 4: 52[741,541] 79[926,463]
    cluster 5: 57[795,59] 83[603,68] 89[749,190]
    cluster 6: 61[267,375] 66[125,211] 72[277,272] 74[222,444] 87[149,304]
    cluster 7: 93[944,835]
    ```

## Testing
Tested on both `merlin` and `eva` servers. This can be built on macOS, but `valgrind` will not work. Most work done in CLion connected to WSL Ubuntu 20.04.
You can run tests on your machine by using `make test`.