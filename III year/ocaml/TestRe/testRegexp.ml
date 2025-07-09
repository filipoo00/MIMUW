open Regexp

module Test(Re : REGEXP) = struct
  module StringSet = Set.Make(String)
  type lang = StringSet.t

  let make_lang (alphabet : string) (n : int) : lang = 
    let alpha_len = String.length alphabet in

    let random_word max_len =
      let len = Random.int (max_len + 1) in
      let buf = Buffer.create len in
      for _i = 1 to len do
        let c = alphabet.[Random.int alpha_len] in
        Buffer.add_char buf c
      done;
      Buffer.contents buf
    in

    let rec build_set i acc = 
      if i = 0 then acc
      else 
        let w = random_word n in
        build_set (i-1) (StringSet.add w acc)
    in

    build_set n StringSet.empty

  let select_accepted (r : Re.t) (l : lang) : lang =
    StringSet.filter (fun w -> Re.matches r w) l

  let test_two (r1 : Re.t) (r2 : Re.t) (l : lang) (_debug : bool) : int * float =
    let start_time = Sys.time () in
    let accepted_r1 = select_accepted r1 l in
    let accepted_r2 = select_accepted r2 l in
    let total_time = Sys.time () -. start_time in

    let diff_r1_r2 = StringSet.diff accepted_r1 accepted_r2 in
    let diff_r2_r1 = StringSet.diff accepted_r2 accepted_r1 in

    let errors = StringSet.cardinal diff_r1_r2 + StringSet.cardinal diff_r2_r1 in

    if _debug then (
      print_endline "===== Debug =====";
      print_endline "Expression r1:";
      Re.debug r1;
      print_endline "Expression r2:";
      Re.debug r2;
  
      print_endline "\nWords accepted by r1:";
      StringSet.iter (fun w -> print_endline w) accepted_r1;
      print_endline "\nWords accepted by r2:";
      StringSet.iter (fun w -> print_endline w) accepted_r2;

      print_endline "=== Debug End ===\n"
    );
    
    (errors, total_time)

  let test () : int * float =

    let total_errors = ref 0 in
    let total_time = ref 0.0 in

    let s_a = String.make 1000000 'a' in
    let s_b = String.make 1000000 'b' in
    let s_c = String.make 1000000 'c' in

    let assert_should_accept word acc =
      if not (StringSet.mem word acc) then (
        incr total_errors;
        (* print_endline ("The word should be accepted: " ^ word) *)
      )
    in

    let assert_should_not_accept word acc =
      if (StringSet.mem word acc) then (
        incr total_errors;
        (* print_endline ("The word shouldn't be accepted: " ^ word) *)
      )
    in

    (* Testy 1 *)
    let re1 = Re.re "ba*b" in
    let s1 = "b" ^ s_a ^ "b" in
    let s2 = "b" ^ s_a ^ "b" ^ s_a ^ "b" in
    let l1 = StringSet.of_list [
      "baabaabaaab";
      "baaaaaaab";
      s1;
      s2
    ] 
    in

    let start_time_test1a = Sys.time () in
    let accepted1 = select_accepted re1 l1 in
    let total_time_test1a = Sys.time () -. start_time_test1a in 

    assert_should_accept "baaaaaaab" accepted1;
    assert_should_accept s1 accepted1;    

    assert_should_not_accept "baabaabaaab" accepted1;
    assert_should_not_accept s2 accepted1;


    let re2 = Re.re "a*b" in
    let s3 = s_a ^ "b" in
    let s4 = s_a in
    let l2 = StringSet.of_list [
      "aaaaaaa";
      "aaaaab";
      "bb";
      s3;
      s4
    ] 
    in

    let start_time_test1b = Sys.time () in
    let accepted2 = select_accepted re2 l2 in
    let total_time_test1b = Sys.time () -. start_time_test1b in 

    assert_should_accept "aaaaab" accepted2;
    assert_should_accept s3 accepted2;

    assert_should_not_accept "aaaaaaa" accepted2;
    assert_should_not_accept "bb" accepted2;
    assert_should_not_accept s4 accepted2;

    let total_time_test1 = total_time_test1a +. total_time_test1b in
    total_time := !total_time +. total_time_test1;

    (* Testy 2 *)
    let re3 = Re.re "a*a" in
    let re4 = Re.re "aa*" in
    let l3 = make_lang "ab" 1000 in
    let (errors, time) = test_two re3 re4 l3 false in
    total_errors := !total_errors + errors;
    total_time := !total_time +. time;

    let re5 = Re.re "(a|b)*(a|b)" in
    let re6 = Re.re "(a|b)(a|b)*" in
    let l4 = make_lang "abc" 1000 in
    let (errors, time) = test_two re5 re6 l4 false in
    total_errors := !total_errors + errors;
    total_time := !total_time +. time;


    (* Testy 3 *)
    let re7 = Re.re "a(a|b)*|(a|b)*b" in
    let re8 = Re.re "(a|b)*b|a(a|b)*" in
    let l5 = make_lang "ab" 1000 in
    let (errors, time) = test_two re7 re8 l5 false in
    total_errors := !total_errors + errors;
    total_time := !total_time +. time;


    (* Testy 4 *)
    let re9 = Re.re "a*b*c*" in
    let s5 = s_a in
    let s6 = s_b in
    let s7 = s_c in
    let s8 = s_a ^ s_b ^ s_c in
    let s9 = s_a ^ "c" ^ s_b ^ s_c in
    let s10 = s_a ^ s_b ^ "a" ^ s_c in
    let s11 = s_a ^ "ba" ^ s_b  ^ s_c in
    let l6 = StringSet.of_list [
      s5;
      s6;
      s7;
      s8;
      s9;
      s10;
      s11;
    ] 
    in

    let start_time_test4a = Sys.time () in
    let accepted3 = select_accepted re9 l6 in
    let total_time_test4a = Sys.time () -. start_time_test4a in 

    assert_should_accept s5 accepted3;
    assert_should_accept s6 accepted3;
    assert_should_accept s7 accepted3;
    assert_should_accept s8 accepted3;

    assert_should_not_accept s9 accepted3;
    assert_should_not_accept s10 accepted3;
    assert_should_not_accept s11 accepted3;    


    let re10 = Re.re "((a|b)*c)*" in
    let s12 = s_c in
    let s13 = "a" ^ s_c ^ s_a ^ "c" in
    let s14 = "" in
    let s15 = s_a in
    let s16 = s_b in

    let l7 = StringSet.of_list [
      s12;
      s13;
      s14;
      s15;
      s16
    ] 
    in


    let start_time_test4b = Sys.time () in
    let accepted4 = select_accepted re10 l7 in
    let total_time_test4b = Sys.time () -. start_time_test4b in 

    assert_should_accept s12 accepted4;
    assert_should_accept s13 accepted4;
    assert_should_accept s14 accepted4;

    assert_should_not_accept s15 accepted4;
    assert_should_not_accept s16 accepted4;

    let total_time_test4 = total_time_test4a +. total_time_test4b in
    total_time := !total_time +. total_time_test4;

    (!total_errors, !total_time)

end