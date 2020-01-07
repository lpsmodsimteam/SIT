// See README.md for license details.

package ram

import java.io.File;
import org.newsclub.net.unix.{AFUNIXSocket, AFUNIXServerSocket}
import org.newsclub.net.unix.AFUNIXSocketAddress

import chisel3.iotesters
import chisel3.iotesters.{ChiselFlatSpec, Driver, PeekPokeTester}

class RAMUnitTester(c: RAM) extends PeekPokeTester(c) {

    private val socketFile: File = new File("/tmp/bruh")
    private val sock: AFUNIXSocket = AFUNIXSocket.newInstance()
    sock.connect(new AFUNIXSocketAddress(socketFile))

    private val ram = c

    // write i*10 for 10 cycles
    for (i <- 0 to 9 by 1) {
        poke(ram.io.address, i)
        poke(ram.io.cs, true)
        poke(ram.io.we, true)
        poke(ram.io.oe, false)
        poke(ram.io.data_in, i * 10)
        step(1)
    }

    // read for 10 cycles
    for (i <- 0 to 9 by 1) {
        poke(ram.io.address, i)
        poke(ram.io.cs, true)
        poke(ram.io.we, false)
        poke(ram.io.oe, true)
        step(1)
        expect(ram.io.data_out, i * 10)
    }

}
