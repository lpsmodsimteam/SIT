package counters

import chisel3._

class counters extends Module {
    val io = IO(new Bundle {
        val op1 = Input(UInt(33.W))
        val op2 = Input(UInt(33.W))
        val out = Output(UInt(33.W))
    })

    io.out := io.op1 * io.op2

}
