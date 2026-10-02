# stm32-uart-driver

Interrupt-driven UART driver for the STM32F407, written bare-metal against CMSIS registers, with a small command shell to exercise it. It is unit-tested on the host and tested end to end in the Renode simulator, so CI runs the real firmware without hardware.

## Features

- Interrupt-driven RX and TX on USART2 through lock-free ring buffers; the main loop never blocks on the UART
- No dynamic allocation; every buffer is statically sized
- RX overruns are counted, never silently dropped
- Command shell: `help`, `led on|off`, `uptime`, `echo <text>`

## Requirements

- `arm-none-eabi-gcc`, CMake 3.25+, Ninja
- [Renode](https://renode.io) for the simulation tests
- Optional hardware: STM32F4 Discovery with a 3.3 V USB-serial adapter on PA2 (TX) and PA3 (RX), 115200 8N1

## Build

```bash
cmake --workflow --preset firmware
```

## Test

```bash
cmake --workflow --preset host-tests       # unit tests on the host
renode-test tests/sim/uart_shell.robot     # firmware in Renode
```

## Run

In the simulator, with the board's UART opened in a terminal window:

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
src/          application, UART driver, ring buffer, shell
platform/     linker script
cmake/        arm-none-eabi toolchain file
tests/unit/   Unity tests run on the host
tests/sim/    Renode platform script and Robot Framework tests
```

## License

MIT
