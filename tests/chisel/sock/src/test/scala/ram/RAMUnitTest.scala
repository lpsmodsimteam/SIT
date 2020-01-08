// See README.md for license details.

package ram

import java.io.File
import java.io.InputStream
import java.io.OutputStream
import java.nio.charset.StandardCharsets.UTF_8

import scala.util.control.Breaks._

import org.newsclub.net.unix.{AFUNIXSocket, AFUNIXServerSocket}
import org.newsclub.net.unix.AFUNIXSocketAddress

import chisel3.iotesters
import chisel3.iotesters.{ChiselFlatSpec, Driver, PeekPokeTester}

class RAMUnitTester(c: RAM) extends PeekPokeTester(c) {

    private val ram = c

    private val socketFile: File = new File("/tmp/bruh")
    private val sock: AFUNIXSocket = AFUNIXSocket.newInstance()
    sock.connect(new AFUNIXSocketAddress(socketFile))

    private val is: InputStream = sock.getInputStream()
    private val os: OutputStream = sock.getOutputStream()

    var buf = new Array[Byte](20)

    private val pid: String = ProcessHandle.current().pid().toString
    // send PID (not really)
    os.write(pid.getBytes("UTF-8"))
    os.flush()


    breakable {

        while (true) {

            var read: Int = is.read(buf)
            var signal = new String(buf, UTF_8)

            var alive: Int = signal.slice(0, 1).toInt
            signal = signal.slice(1, read)

            if (alive == 0) {
                break
            }

            var address: Int = signal.slice(0, 3).toInt
            var cs: Int = signal.slice(3, 4).toInt
            var we: Int = signal.slice(4, 5).toInt
            var oe: Int = signal.slice(5, 6).toInt
            var data_in: Int = signal.slice(6, 9).toInt
            // println("alive: " + alive + " address: " + address + " cs: " + cs + " we: " + we + " oe: " + oe + " data_in: " + data_in)

            poke(ram.io.address, address)
            poke(ram.io.cs, (cs == 1))
            poke(ram.io.we, (we == 1))
            poke(ram.io.oe, (oe == 1))
            poke(ram.io.data_in, data_in)
            step(1)

            var data_out: String = peek(ram.io.data_out).toString

            os.write(data_out.getBytes("UTF-8"))
            os.flush()

        }
    }

}
