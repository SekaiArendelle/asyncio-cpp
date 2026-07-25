# asyncio-cpp

A C++23 async/await event loop library built on coroutines (C++20 modules).

## Build

```bash
pixi run configure
pixi run build
pixi run install
```

## Test

```bash
pixi run test
```

## Sanitizers

Enable AddressSanitizer or UndefinedBehaviorSanitizer via the `sanitizer` task argument:

```bash
# AddressSanitizer
pixi run test address

# UndefinedBehaviorSanitizer
pixi run test undefined

# Both
pixi run test 'address,undefined'
```

On Linux/macOS the sanitized tests run directly.  On Windows the ASan runtime
(`clang_rt.asan_dynamic-x86_64.dll`) must be on the `PATH` — it lives under
`.pixi/envs/default/Library/bin/` in the pixi environment.

## Examples

```bash
pixi run example coro_func
```

## Clean

```bash
pixi run clean
pixi clean
```
