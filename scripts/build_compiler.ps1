$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$rootDir = Split-Path -Parent $scriptDir
$compilerDir = Join-Path $rootDir "backend\compiler"
$buildDir = Join-Path $compilerDir "build"
$serverCompilerDir = Join-Path $rootDir "web\src\server\compiler"

if (-not (Test-Path $buildDir)) {
  New-Item -ItemType Directory -Path $buildDir | Out-Null
}

if (-not (Test-Path $serverCompilerDir)) {
  New-Item -ItemType Directory -Path $serverCompilerDir | Out-Null
}

Push-Location $buildDir
try {
  Write-Host "==> Running Flex (lexer)..."
  flex "..\lexer\lexer.l"

  Write-Host "==> Running Bison (parser)..."
  bison -d -o "parser.tab.c" "..\parser\parser.y"

  Write-Host "==> Compiling AstroScript compiler..."
  g++ `
    "lex.yy.c" `
    "parser.tab.c" `
    "..\semantic\symbol_table.cpp" `
    "..\ir\tac.cpp" `
    "..\main.cpp" `
    -I".." `
    -I"." `
    -std=c++17 `
    -o "astroscript.exe"

  $sourceBinary = Join-Path $buildDir "astroscript.exe"
  $targetBinary = Join-Path $serverCompilerDir "astroscript-windows.exe"
  Copy-Item -Path $sourceBinary -Destination $targetBinary -Force

  Write-Host "==> Build complete: $buildDir\astroscript.exe"
  Write-Host "==> Synced Windows binary to: $targetBinary"
}
finally {
  Pop-Location
}
