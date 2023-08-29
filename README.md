# sqlite-vectordb

## Example



## Build
At the root of the directory, create `build` folder:

```
mkdir build

Build project:
```

```
cmake --build build;
```

This will build the extension in `build/libvector_extension.so`. 

Then start sqlite and use the `.load` command to start using the extension:
```
sqlite> .load {your/path/to/../}libvector_extension.so
```

## Testing
After building the project in the Build section, run from the `build` folder:
```
./test_extension  # Integration test for the extension
./test_vec # Unit tests
```

## API Reference

|Function|Description|
|-|-|
|VECTOR(c1, c2, ...)| Constructs an n-dimensional vector from `c1`, `c2`, ... `cn`. Arguments must be REAL data type.
|VECTOR_ADD(v, w)| Add two vectors `v` and `w` and returns the result.|
|VECTOR_SUB(v, w)| Subtract `w` from `v` and returns the result.|
|VECTOR_NN(v, table.column, k)|Finds the `k`-nearest neighbors in a set of vectors stored in `table.column`
