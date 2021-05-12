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
    env: ['BENCH_TYPES=TrivialPointer'],
)

slow_timeout = meson.version().version_compare('>= 0.57.0') ? -1 : 18000

benchmark('bench-full', bench,
    timeout: slow_timeout,
    env: ['BENCH_TYPES=full'],
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
    'erase_back_swap',
    'erase_back_value',
    'erase_back_value_swap',
    'erase_front',
    'erase_front_swap',
    'erase_front_value',
    'erase_front_value_swap',
    'erase_full',
    'erase_middle',
    'erase_middle_swap',
    'erase_middle_value',
    'erase_middle_value_swap',
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
    env: [
        'BENCH_NAMES='+b,
        'BENCH_TYPES=TrivialPointer'
    ],
    suite: ['single'],
)
endforeach

add_test_setup('default',
    is_default: true,
    exclude_suites: ['slow', 'single'], # Needs meson 0.57
)
