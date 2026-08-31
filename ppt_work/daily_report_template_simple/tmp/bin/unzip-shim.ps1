param(
  [Parameter(Position=0)] [string] $Mode,
  [Parameter(Position=1)] [string] $ZipPath,
  [Parameter(Position=2)] [string] $EntryName
)

Add-Type -AssemblyName System.IO.Compression.FileSystem

if (($Mode -eq '-Z1' -or $Mode -eq 'Z1') -and $ZipPath) {
  $zip = [System.IO.Compression.ZipFile]::OpenRead($ZipPath)
  try {
    foreach ($entry in $zip.Entries) {
      [Console]::Out.WriteLine($entry.FullName)
    }
  } finally {
    $zip.Dispose()
  }
  exit 0
}

if (($Mode -eq '-p' -or $Mode -eq 'p') -and $ZipPath -and $EntryName) {
  $zip = [System.IO.Compression.ZipFile]::OpenRead($ZipPath)
  try {
    $entry = $zip.Entries | Where-Object { $_.FullName -eq $EntryName } | Select-Object -First 1
    if (-not $entry) { exit 11 }
    $stream = $entry.Open()
    try {
      $out = [Console]::OpenStandardOutput()
      $buffer = New-Object byte[] 8192
      while (($read = $stream.Read($buffer, 0, $buffer.Length)) -gt 0) {
        $out.Write($buffer, 0, $read)
      }
      $out.Flush()
    } finally {
      $stream.Dispose()
    }
  } finally {
    $zip.Dispose()
  }
  exit 0
}

Write-Error 'usage: unzip -Z1 <zip> | unzip -p <zip> <entry>'
exit 2
