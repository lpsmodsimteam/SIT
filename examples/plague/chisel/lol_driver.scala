package lol

import java.io.{File, InputStream, OutputStream}
// import java.lang.ProcessHandle
import java.nio.charset.StandardCharsets.UTF_8
import scala.util.control.Breaks.{break, breakable}

// import org.newsclub.net.unix.{AFUNIXSocket, AFUNIXSocketAddress, AFUNIXServerSocket}
import chisel3.iotesters

class lolDriver(c: lol, args: Array[String]) extends iotesters.PeekPokeTester(c) {

    private val uut = c

    poke(uut.io.op1, 100000000)
    poke(uut.io.op2, 100000000)
    step(1)

    println(peek(uut.io.out).toString)

}

object lolMain extends App {
  iotesters.Driver.execute(args, () => new lol) {
    c => new lolDriver(c, args)
  }
}
