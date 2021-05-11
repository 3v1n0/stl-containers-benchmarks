project('stl-containers-benchmarks', 'cpp',
    version: '0.2',
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

slow_timeout = meson.version().version_compare('>= 0.57.0') ? -1 : 18000

benchmark('bench-all', bench,
    timeout: slow_timeout,
    env: ['BENCH=all'],
    suite: ['slow'],
)

single_benchmarks = [
    'destruction',
    'emplace_back',
    'emplace_front',
    'erase1',
    'erase10',
    'erase25',
    'erase50',
    'erase_back',
    'erase_front',
    'erase_full',
    'erase_middle',
    'fastest_insertion',
    'fill_back',
    'fill_front',
    'linear_search',
    'number_crunching',
    'random_insert',
    'random_remove',
    'sort',
    'traversal',
    'traversal_and_clear',
    'write',
    'find',
]

foreach b: single_benchmarks
    benchmark(b, bench,
    timeout: slow_timeout,
    env: ['BENCH='+b],
    suite: ['single'],
)
endforeach

add_test_setup('default',
    is_default: true,
    exclude_suites: ['slow', 'single'], # Needs meson 0.57
)
