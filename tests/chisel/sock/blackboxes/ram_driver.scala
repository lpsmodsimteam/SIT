package ram

import java.io.{File, InputStream, OutputStream}
import java.nio.charset.StandardCharsets.UTF_8
import scala.util.control.Breaks._

import org.newsclub.net.unix.{AFUNIXSocket, AFUNIXSocketAddress, AFUNIXServerSocket}
import chisel3.iotesters

class ramUnitTester(c: ram, args: Array[String]) extends iotesters.PeekPokeTester(c) {

    private val ram = c

    var address: Int = _
    var cs: Boolean = _
    var we: Boolean = _
    var oe: Boolean = _
    var data_in: Int = _
    var data_out: String = _

    private val _sock: AFUNIXSocket = AFUNIXSocket.newInstance()
    _sock.connect(new AFUNIXSocketAddress(new File(args(0))))

    private val istream: InputStream = _sock.getInputStream()
    private val ostream: OutputStream = _sock.getOutputStream()

    ostream.write(ProcessHandle.current().pid().toString.getBytes("UTF-8"))
    ostream.flush()

    var alive: Int = _
    var _buf = new Array[Byte](10)
    var read: Int = _
    var signal: String = _

    breakable {

        while (true) {

            read = istream.read(_buf)
            signal = new String(_buf, UTF_8)

            alive = signal.slice(0, 1).toInt
            signal = signal.slice(1, read)

            if (alive == 0) {
                break
            }

            address = signal.slice(0, 3).toInt
            cs = signal.slice(3, 4).toInt == 1
            we = signal.slice(4, 5).toInt == 1
            oe = signal.slice(5, 6).toInt == 1
            data_in = signal.slice(6, 9).toInt

            poke(ram.io.address, address)
            poke(ram.io.cs, cs)
            poke(ram.io.we, we)
            poke(ram.io.oe, oe)
            poke(ram.io.data_in, data_in)
            step(1)

            data_out = peek(ram.io.data_out).toString

            ostream.write(data_out.getBytes("UTF-8"))
            ostream.flush()

        }
    }

    _buf = Array.empty[Byte]

}

object ramMain extends App {
  iotesters.Driver.execute(args, () => new ram) {
    c => new ramUnitTester(c, args)
  }
}
