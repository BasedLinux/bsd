function _bsd_complete
  # Get the current command up to a cursor.
  # - Behaves correctly even with pipes and nested in commands like env.
  # - TODO: Returns the command verbatim (does not interpolate variables).
  #   That might not be optimal for arguments like -f.
  set -l bsd_args (commandline --current-process --tokenize --cut-at-cursor)
  # --cut-at-cursor with --tokenize removes the current token so we need to add it separately.
  # https://github.com/fish-shell/fish-shell/issues/7375
  # Can be an empty string.
  set -l current_token (commandline --current-token --cut-at-cursor)

  # Bsd wants the index of the argv item to complete but the $bsd_args variable
  # also contains the program name (argv[0]) so we would need to subtract 1.
  # But the variable also misses the current token so it cancels out.
  set -l bsd_arg_to_complete (count $bsd_args)

  env NIX_GET_COMPLETIONS=$bsd_arg_to_complete $bsd_args $current_token
end

function _bsd_accepts_files
  set -l response (_bsd_complete)
  test $response[1] = 'filenames'
end

function _bsd
  set -l response (_bsd_complete)
  # Skip the first line since it handled by _bsd_accepts_files.
  # Tail lines each contain a command followed by a tab character and, optionally, a description.
  # This is also the format fish expects.
  string collect -- $response[2..-1]
end

# Disable file path completion if paths do not belong in the current context.
complete --command bsd --condition 'not _bsd_accepts_files' --no-files

complete --command bsd --arguments '(_bsd)'
