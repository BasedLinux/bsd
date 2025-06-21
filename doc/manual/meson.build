project('bsd-manual',
  version : files('.version'),
  meson_version : '>= 1.1',
  license : 'LGPL-2.1-or-later',
)

bsd = find_program('bsd', native : true)

mdbook = find_program('mdbook', native : true)
bash = find_program('bash', native : true)

pymod = import('python')
python = pymod.find_installation('python3')

bsd_env_for_docs = {
  'HOME': '/dummy',
  'NIX_CONF_DIR': '/dummy',
  'NIX_SSL_CERT_FILE': '/dummy/no-ca-bundle.crt',
  'NIX_STATE_DIR': '/dummy',
  'NIX_CONFIG': 'cores = 0',
}

bsd_for_docs = [bsd, '--experimental-features', 'bsd-command']
bsd_eval_for_docs_common = bsd_for_docs + [
  'eval',
  '-I', 'bsd=' + meson.current_source_dir(),
  '--store', 'dummy://',
  '--impure',
]
bsd_eval_for_docs = bsd_eval_for_docs_common + '--raw'

conf_file_json = custom_target(
  command : bsd_for_docs + ['config', 'show', '--json'],
  capture : true,
  output : 'conf-file.json',
  env : bsd_env_for_docs,
)

language_json = custom_target(
  command: [bsd, '__dump-language'],
  output : 'language.json',
  capture : true,
  env : bsd_env_for_docs,
)

bsd3_cli_json = custom_target(
  command : [bsd, '__dump-cli'],
  capture : true,
  output : 'bsd.json',
  env : bsd_env_for_docs,
)

generate_manual_deps = files(
  'generate-deps.py',
)

# Generates types
subdir('source/store')
# Generates builtins.md and builtin-constants.md.
subdir('source/language')
# Generates new-cli pages, experimental-features-shortlist.md, and conf-file.md.
subdir('source/command-ref')
# Generates experimental-feature-descriptions.md.
subdir('source/development')
# Generates rl-next-generated.md.
subdir('source/release-notes')
subdir('source')

# Hacky way to figure out if `bsd` is an `ExternalProgram` or
# `Executable`. Only the latter can occur in custom target input lists.
if bsd.full_path().startswith(meson.build_root())
  bsd_input = bsd
else
  bsd_input = []
endif

manual = custom_target(
  'manual',
  command : [
    bash,
    '-euo', 'pipefail',
    '-c',
    '''
        @0@ @INPUT0@ @CURRENT_SOURCE_DIR@ > @DEPFILE@
        @0@ @INPUT1@ summary @2@ < @CURRENT_SOURCE_DIR@/source/SUMMARY.md.in > @2@/source/SUMMARY.md
        sed -e 's|@version@|@3@|g' < @INPUT2@ > @2@/book.toml
        rsync -r --include='*.md' @CURRENT_SOURCE_DIR@/ @2@/
        (cd @2@; RUST_LOG=warn @1@ build -d @2@ 3>&2 2>&1 1>&3) | { grep -Fv "because fragment resolution isn't implemented" || :; } 3>&2 2>&1 1>&3
        rm -rf @2@/manual
        mv @2@/html @2@/manual
        find @2@/manual -iname meson.build -delete
    '''.format(
      python.full_path(),
      mdbook.full_path(),
      meson.current_build_dir(),
      meson.project_version(),
    ),
  ],
  input : [
    generate_manual_deps,
    'substitute.py',
    'book.toml.in',
    'anchors.jq',
    'custom.css',
    bsd3_cli_files,
    experimental_features_shortlist_md,
    experimental_feature_descriptions_md,
    types_dir,
    conf_file_md,
    builtins_md,
    rl_next_generated,
    summary_rl_next,
    bsd_input,
  ],
  output : [
    'manual',
    'markdown',
  ],
  depfile : 'manual.d',
  env : {
    'RUST_LOG': 'info',
    'MDBOOK_SUBSTITUTE_SEARCH': meson.current_build_dir() / 'source',
  },
)
manual_html = manual[0]
manual_md = manual[1]

install_subdir(
  manual_html.full_path(),
  install_dir : get_option('datadir') / 'doc/bsd',
)

bsd_nested_manpages = [
  [ 'bsd-env',
    [
      'delete-generations',
      'install',
      'list-generations',
      'query',
      'rollback',
      'set-flag',
      'set',
      'switch-generation',
      'switch-profile',
      'uninstall',
      'upgrade',
    ],
  ],
  [ 'bsd-store',
    [
      'add-fixed',
      'add',
      'delete',
      'dump-db',
      'dump',
      'export',
      'gc',
      'generate-binary-cache-key',
      'import',
      'load-db',
      'optimise',
      'print-env',
      'query',
      'read-log',
      'realise',
      'repair-path',
      'restore',
      'serve',
      'verify',
      'verify-path',
    ],
  ],
]

foreach command : bsd_nested_manpages
  foreach page : command[1]
    title = command[0] + ' --' + page
    section = '1'
    custom_target(
      command : [
        bash,
        files('./render-manpage.sh'),
        '--out-no-smarty',
        title,
        section,
        '@INPUT0@/command-ref' / command[0] / (page + '.md'),
        '@OUTPUT0@',
      ],
      input : [
        manual_md,
        bsd_input,
      ],
      output : command[0] + '-' + page + '.1',
      install : true,
      install_dir : get_option('mandir') / 'man1',
    )
  endforeach
endforeach

bsd3_manpages = [
  'bsd3-build',
  'bsd3-bundle',
  'bsd3-config',
  'bsd3-config-check',
  'bsd3-config-show',
  'bsd3-copy',
  'bsd3-daemon',
  'bsd3-derivation-add',
  'bsd3-derivation',
  'bsd3-derivation-show',
  'bsd3-develop',
  'bsd3-edit',
  'bsd3-env-shell',
  'bsd3-eval',
  'bsd3-flake-archive',
  'bsd3-flake-check',
  'bsd3-flake-clone',
  'bsd3-flake-info',
  'bsd3-flake-init',
  'bsd3-flake-lock',
  'bsd3-flake',
  'bsd3-flake-metadata',
  'bsd3-flake-new',
  'bsd3-flake-prefetch',
  'bsd3-flake-show',
  'bsd3-flake-update',
  'bsd3-fmt',
  'bsd3-hash-file',
  'bsd3-hash',
  'bsd3-hash-convert',
  'bsd3-hash-path',
  'bsd3-hash-to-base16',
  'bsd3-hash-to-base32',
  'bsd3-hash-to-base64',
  'bsd3-hash-to-sri',
  'bsd3-help',
  'bsd3-help-stores',
  'bsd3-key-convert-secret-to-public',
  'bsd3-key-generate-secret',
  'bsd3-key',
  'bsd3-log',
  'bsd3-nar-cat',
  'bsd3-nar-dump-path',
  'bsd3-nar-ls',
  'bsd3-nar-pack',
  'bsd3-nar',
  'bsd3-path-info',
  'bsd3-print-dev-env',
  'bsd3-profile',
  'bsd3-profile-add',
  'bsd3-profile-diff-closures',
  'bsd3-profile-history',
  'bsd3-profile-list',
  'bsd3-profile-remove',
  'bsd3-profile-rollback',
  'bsd3-profile-upgrade',
  'bsd3-profile-wipe-history',
  'bsd3-realisation-info',
  'bsd3-realisation',
  'bsd3-registry-add',
  'bsd3-registry-list',
  'bsd3-registry',
  'bsd3-registry-pin',
  'bsd3-registry-remove',
  'bsd3-repl',
  'bsd3-run',
  'bsd3-search',
  'bsd3-store-add',
  'bsd3-store-add-file',
  'bsd3-store-add-path',
  'bsd3-store-cat',
  'bsd3-store-copy-log',
  'bsd3-store-copy-sigs',
  'bsd3-store-delete',
  'bsd3-store-diff-closures',
  'bsd3-store-dump-path',
  'bsd3-store-gc',
  'bsd3-store-info',
  'bsd3-store-ls',
  'bsd3-store-make-content-addressed',
  'bsd3-store',
  'bsd3-store-optimise',
  'bsd3-store-path-from-hash-part',
  'bsd3-store-prefetch-file',
  'bsd3-store-repair',
  'bsd3-store-sign',
  'bsd3-store-verify',
  'bsd3-upgrade-bsd',
  'bsd3-why-depends',
  'bsd',
]

foreach page : bsd3_manpages
  section = '1'
  custom_target(
    command : [
      bash,
      '@INPUT0@',
      page,
      section,
      '@INPUT1@/command-ref/new-cli/@0@.md'.format(page),
      '@OUTPUT@',
    ],
    input : [
      files('./render-manpage.sh'),
      manual_md,
      bsd_input,
    ],
    output : page + '.1',
    install : true,
    install_dir : get_option('mandir') / 'man1',
  )
endforeach

bsd_manpages = [
  [ 'bsd-env', 1 ],
  [ 'bsd-store', 1 ],
  [ 'bsd-build', 1 ],
  [ 'bsd-shell', 1 ],
  [ 'bsd-instantiate', 1 ],
  [ 'bsd-collect-garbage', 1 ],
  [ 'bsd-prefetch-url', 1 ],
  [ 'bsd-channel', 1 ],
  [ 'bsd-hash', 1 ],
  [ 'bsd-copy-closure', 1 ],
  [ 'bsd.conf', 5, conf_file_md.full_path() ],
  [ 'bsd-daemon', 8 ],
  [ 'bsd-profiles', 5, 'files/profiles.md' ],
]

foreach entry : bsd_manpages
  title = entry[0]
  # bsd.conf.5 and bsd-profiles.5 are based off of conf-file.md and files/profiles.md,
  # rather than a stem identical to its mdbook source.
  # Therefore we use an optional third element of this array to override the name pattern
  md_file = entry.get(2, title + '.md')
  section = entry[1].to_string()
  md_file_resolved = join_paths('@INPUT1@/command-ref/', md_file)
  custom_target(
    command : [
      bash,
      '@INPUT0@',
      title,
      section,
      md_file_resolved,
      '@OUTPUT@',
    ],
    input : [
      files('./render-manpage.sh'),
      manual_md,
      entry.get(3, []),
      bsd_input,
    ],
    output : '@0@.@1@'.format(entry[0], entry[1]),
    install : true,
    install_dir : get_option('mandir') / 'man@0@'.format(entry[1]),
  )
endforeach
