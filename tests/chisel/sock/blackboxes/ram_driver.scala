package ram

import java.io.{File, InputStream, OutputStream}
import java.nio.charset.StandardCharsets.UTF_8
import scala.util.control.Breaks.{break, breakable}

import org.newsclub.net.unix.{AFUNIXSocket, AFUNIXSocketAddress, AFUNIXServerSocket}
import chisel3.iotesters

class ramUnitTester(c: ram, args: Array[String]) extends iotesters.PeekPokeTester(c) {

    private val ram = c

    private val _sock: AFUNIXSocket = AFUNIXSocket.newInstance()
    _sock.connect(new AFUNIXSocketAddress(new File(args(0))))

    private val istream: InputStream = _sock.getInputStream()
    private val ostream: OutputStream = _sock.getOutputStream()

    ostream.write(ProcessHandle.current().pid().toString.getBytes("UTF-8"))
    ostream.flush()

    var alive: Int = _
    var signal: String = _
    var _buf = new Array[Byte](10)
    var _read: Int = _

    breakable {

        while (true) {

            _read = istream.read(_buf)
            signal = new String(_buf, UTF_8)

            alive = signal.slice(0, 1).toInt
            signal = signal.slice(1, _read)

            if (alive == 0) {
                break
            }

            poke(ram.io.address, signal.slice(0, 3).toInt)
            poke(ram.io.cs, signal.slice(3, 4).toInt == 1)
            poke(ram.io.we, signal.slice(4, 5).toInt == 1)
            poke(ram.io.oe, signal.slice(5, 6).toInt == 1)
            poke(ram.io.data_in, signal.slice(6, 9).toInt)
            step(1)

            ostream.write((peek(ram.io.data_out).toString).getBytes("UTF-8"))
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
