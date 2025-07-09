open TestRe

module T1 = TestRegexp.Test(ReSimple)
module T2 = TestRegexp.Test(ReKit)

type tests = unit -> int * float

let testers : (string * tests) list = [
  ("ReSimple", T1.test);
  ("ReKit",    T2.test);
]

let () =
  Random.self_init ();
  List.iter (fun (modname, testfunc) ->
    let (errors, time) = testfunc () in
    Printf.printf "Module %s: total errors: %d, total time: %f\n"
      modname errors time
  ) testers
