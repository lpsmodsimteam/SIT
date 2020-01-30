package counters

import java.io.{File, InputStream, OutputStream}
// import java.lang.ProcessHandle
import java.nio.charset.StandardCharsets.UTF_8
import scala.util.control.Breaks.{break, breakable}

// import org.newsclub.net.unix.{AFUNIXSocket, AFUNIXSocketAddress, AFUNIXServerSocket}
import chisel3.iotesters

class countersDriver(c: counters, args: Array[String]) extends iotesters.PeekPokeTester(c) {

    private val uut = c

    poke(uut.io.op1, 2)
    poke(uut.io.op2, 10)
    step(1)

    println(peek(uut.io.out).toString)

}

object countersMain extends App {
  iotesters.Driver.execute(args, () => new counters) {
    c => new countersDriver(c, args)
  }
}
