This is a set of STL containers benchmarks forked from
[Baptiste Wicht articles code](https://github.com/wichtounet/articles.git).

### Build and run

    meson _build
    meson test -C _build --benchmark bench -v

Results are saved in the `_build` directory in html format, using google
graphs (kudos to Baptiste!).

### License

All the code is licensed under the terms of the MIT license. It means that you
can use this code in your software, even proprietary, as long as you retain the
copyright notice.
