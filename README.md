This is a set of STL containers benchmarks forked from
[Baptiste Wicht articles code](https://github.com/wichtounet/articles.git).

### Build and run

```bash
meson _build
meson test -C _build --benchmark bench -v

# A single benchmark can be ran using the bench name as listed via:
#meson test -C _build --benchmark --list
```

Results are saved in the `_build` directory in html format, using google
graphs (kudos to Baptiste!).

### Results

You can see results of these tests (running in github actions) in the
[releases page](https://github.com/3v1n0/stl-containers-benchmarks/releases) of
this repo.

### License

All the code is licensed under the terms of the MIT license. It means that you
can use this code in your software, even proprietary, as long as you retain the
copyright notice.
