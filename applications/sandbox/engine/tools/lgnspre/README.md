Modular Preprocessor for Legion Shaders

```
Preprocess Legion Engine Shaders into pure glsl

Usage:
 lgnspre <file> [-D defines ...] [-I includes ...] [options] [-v | -vv | -vvv]

Options:
  -D ...                        additional defines (use = to assign a value)
  -I ...                        additional includes
  -f --format=(1file,nfiles)    output format [default: nfiles]
  -o --output=(file,stdout)     output location [default: file]
  -v                            verbose output level
```

TODO:
- [x] output file format 1file needs to be implemented
- [x] output location stdout needs to be implemented
- [x] shader corlib needs to be fleshed out (maybe a task of legion-rendering instead?)

Currently no TODOs yeay :)
Make a feature request Issue if you would like to see more
