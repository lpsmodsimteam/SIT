package ram

import chisel3._

class ram extends Module {
    val DATA_WIDTH: Int = 8
    val ADDR_WIDTH: Int = 8
    val RAM_DEPTH: Int = 1 << 8
    val io = IO(new Bundle {
        val address  = Input(UInt(ADDR_WIDTH.W))
        val cs       = Input(Bool())
        val we       = Input(Bool())
        val oe       = Input(Bool())
        val data_in  = Input(UInt(DATA_WIDTH.W))
        val data_out = Output(UInt(DATA_WIDTH.W))
    })

    val data_out_wire = Reg(UInt(DATA_WIDTH.W))
    val mem = Mem(RAM_DEPTH, UInt(DATA_WIDTH.W))
    data_out_wire := DontCare

    when (io.cs) {
        when(!io.we && io.oe) {
            // read
            data_out_wire := mem.read(io.address)
        }
        .otherwise {
            // write
            mem.write(io.address, io.data_in)
        }
    }

    io.data_out := data_out_wire

}
