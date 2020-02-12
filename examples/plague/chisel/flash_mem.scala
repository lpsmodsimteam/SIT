package flash_mem

import chisel3._

class flash_mem extends Module {
    val DATAW: Int = 25
    val ADDRW: Int = 25
    val FLASHD: Int = 1 << DATAW
    val io = IO(new Bundle {
        val address  = Input(UInt(ADDRW.W))
        val cs       = Input(Bool())
        val we       = Input(Bool())
        val oe       = Input(Bool())
        val data_in  = Input(UInt(DATAW.W))
        val data_out = Output(UInt(DATAW.W))
    })

    val data_out_wire = Reg(UInt(DATAW.W))
    val mem = Mem(FLASHD, UInt(DATAW.W))
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
