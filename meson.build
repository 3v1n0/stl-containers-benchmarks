project('stl-containers-benchmarks', 'cpp',
    version: '0.1.1',
    license: 'MIT',
    meson_version: '>= 0.52.0',
    default_options: [
        'cpp_std=c++17',
        'buildtype=release',
        'debug=false',
        'warning_level=2',
    ],
)

includes = include_directories('include')
bench = executable('bench',
    'src/bench.cpp',
    'src/demangle.cpp',
    'src/graphs.cpp',
    include_directories: includes,
)

benchmark('bench', bench,
    timeout: 600,
    suite: ['normal'],
)

slow_timeout = meson.version().version_compare('>= 0.3.0') ? -1 : 18000

benchmark('bench-all', bench,
    timeout: slow_timeout,
    env: ['BENCH=all'],
    suite: ['slow'],
)

add_test_setup('default',
    is_default: true,
    exclude_suites: ['slow'], # Needs meson 0.57
)
