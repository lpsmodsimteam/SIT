def scalacOptionsVersion(scalaVersion: String): Seq[String] = {
  Seq() ++ {
      // If we're building with Scala > 2.11, enable the compile option
      //  switch to support our anonymous Bundle definitions:
      //  https://github.com/scala/bug/issues/10047
      CrossVersion.partialVersion(scalaVersion) match {
          case Some((2, scalaMajor: Long)) if scalaMajor < 12 => Seq()
          case _ => Seq("-Xsource:2.11")
        }
    }
}

def javacOptionsVersion(scalaVersion: String): Seq[String] = {
  Seq() ++ {
    // Scala 2.12 requires Java 8. We continue to generate
    //  Java 7 compatible code for Scala 2.11
    //  for compatibility with old clients.
    CrossVersion.partialVersion(scalaVersion) match {
      case Some((2, scalaMajor: Long)) if scalaMajor < 12 =>
        Seq("-source", "1.7", "-target", "1.7")
      case _ =>
        Seq("-source", "1.8", "-target", "1.8")
    }
  }
}

name := "ram"
version := "3.2.0"
scalaVersion := "2.12.10"
crossScalaVersions := Seq("2.12.10", "2.11.12")

scalacOptions ++= scalacOptionsVersion(scalaVersion.value)
javacOptions ++= javacOptionsVersion(scalaVersion.value)

resolvers ++= Seq(
  Resolver.sonatypeRepo("snapshots"),
  Resolver.sonatypeRepo("releases")
)
libraryDependencies += "edu.berkeley.cs" %% "chisel3" % "3.2-SNAPSHOT"
libraryDependencies += "edu.berkeley.cs" %% "chisel-iotesters" % "1.3-SNAPSHOT"
// sourcesInBase := false
// scalaSource in Compile := baseDirectory.value / "../sock"
// unmanagedSourceDirectories in Compile += baseDirectory.value / "../common"
