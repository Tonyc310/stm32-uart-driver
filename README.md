# stm32-uart-driver

[![CI](https://github.com/Tonyc310/stm32-uart-driver/actions/workflows/ci.yml/badge.svg)](https://github.com/Tonyc310/stm32-uart-driver/actions/workflows/ci.yml)

Interrupt-driven UART driver for the STM32F407, written bare-metal against CMSIS registers, with a small command shell to exercise it and binary telemetry on a second port. It is unit-tested on the host and tested end to end in the Renode simulator, so CI runs the real firmware without hardware.

## Features

- Interrupt-driven RX and TX on USART2 through lock-free ring buffers; the main loop never blocks on the UART
- No dynamic allocation; every buffer is statically sized
- RX overruns are counted, never silently dropped
- Command shell: `help`, `led on|off`, `uptime`, `echo <text>`, `stats`
- Binary telemetry on USART3 once a second: COBS-framed, CRC-16-checked packets described in [telemetry.toml](telemetry.toml)

## Example session

```
stm32-uart-driver, type help
> help
help          list commands
led on|off    switch the green LED
uptime        milliseconds since reset
echo <text>   print text back
stats         console byte counters
> led on
> uptime
3511 ms
> echo hello from the STM32
hello from the STM32
> stats
rx bytes:   51
tx bytes:   302
rx dropped: 0
> blink
unknown command: blink (try help)
```

## Requirements

- `arm-none-eabi-gcc`, CMake 3.25+, Ninja
- [Renode](https://renode.io) 1.17 for the simulation tests, with `renode-test` on `PATH` and its Python dependencies installed (`pip install -r <renode>/tests/requirements.txt`)
- Optional hardware: STM32F4 Discovery with 3.3 V USB-serial adapters, 115200 8N1: console on PA2 (TX) and PA3 (RX), telemetry on PD8 (TX)

## Build

```bash
cmake --workflow --preset firmware
```

## Test

```bash
cmake --workflow --preset host-tests    # unit tests on the host
cmake --workflow --preset sim-tests     # firmware tests in Renode
```

## Run

In the simulator, after building. The script opens the board's UART in a terminal window; type `start` in the Renode monitor to boot:

```bash
renode tests/sim/stm32f4.resc
```

On hardware:

```bash
openocd -f board/stm32f4discovery.cfg \
  -c "program build/firmware/stm32-uart-driver.elf verify reset exit"
```

## Layout

```
src/            application, UART driver, ring buffer, shell, telemetry framing
platform/       linker script
cmake/          arm-none-eabi toolchain file
tests/unit/     Unity tests run on the host
tests/sim/      Renode platform script and Robot Framework tests
.github/        CI: format check, host tests, firmware build, Renode tests
telemetry.toml  telemetry message layouts
```

## License

MIT
