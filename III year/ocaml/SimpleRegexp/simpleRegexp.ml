open SimpleRegexpDef

type 'c reg = 'c SimpleRegexpDef.reg

let rec simpl (r : 'c reg) : 'c reg = 
  match r with
  | Or (r1, Empty) -> simpl r1
  | Or (Empty, r2) -> simpl r2
  | Concat (_, Empty) -> Empty
  | Concat (Empty, _) -> Empty
  | Concat (r1, Eps) -> simpl r1
  | Concat (Eps, r2) -> simpl r2

  | Lit a -> Lit a
  | Concat (r1, r2) -> Concat (simpl r1, simpl r2)
  | Or (r1, r2) -> Or (simpl r1, simpl r2)
  | Star r -> Star (simpl r)
  | Eps -> Eps
  | Empty -> Empty

let rec nullable (r : 'c reg) : bool = 
  match r with
  | Lit _ -> false
  | Concat (r1, r2) -> (nullable r1) && (nullable r2)
  | Or (r1, r2) -> (nullable r1) || (nullable r2)
  | Star _ -> true
  | Eps -> true
  | Empty -> false

let rec empty (r : 'c reg) : bool = 
  match r with
  | Lit _ -> false
  | Concat (r1, r2) -> (empty r1) || (empty r2)
  | Or (r1, r2) -> (empty r1) && (empty r2)
  | Star _ -> false
  | Eps -> false
  | Empty -> true

let rec der (a : 'c) (r : 'c reg) : 'c reg = 
  match r with
  | Lit c -> if c = a then Eps else Empty
  | Concat (r1, r2) ->
    let left = Concat (der a r1, r2) in
    if nullable r1 then
      Or (left, der a r2)
    else
      left
  | Or (r1, r2) ->
      Or (der a r1, der a r2)
  | Star r -> Concat (der a r, Star r)
  | Eps -> Empty
  | Empty -> Empty

let rec ders (v : 'c list) (r : 'c reg) : 'c reg = 
  match v with
  | [] -> r
  | a :: v' -> ders v' (simpl (der a r))

let accept (r : 'c reg) (w : 'c list) : bool =
  nullable (ders w r)

let rec repr (f : 'c -> string) (r : 'c reg) : string =
  let need_parens parent child =
    match parent, child with
    | Or _, Or _ -> false
    | Concat _, Concat _ -> false

    | Or _, Concat _ -> false
    | Or _, Star _ -> false

    | Concat _, Star _ -> false

    | Star _, (Lit _ | Eps | Empty) -> false
    | _, (Lit _ | Eps | Empty) -> false

    | _ -> true
  in
  match r with
  | Lit c -> f c
  | Concat (r1, r2) ->
      let s1 = if need_parens r r1 then "(" ^ repr f r1 ^ ")" else repr f r1 in
      let s2 = if need_parens r r2 then "(" ^ repr f r2 ^ ")" else repr f r2 in
      s1 ^ s2
  | Or (r1, r2) ->
      let s1 = if need_parens r r1 then "(" ^ repr f r1 ^ ")" else repr f r1 in
      let s2 = if need_parens r r2 then "(" ^ repr f r2 ^ ")" else repr f r2 in
      s1 ^ "|" ^ s2
  | Star r1 ->
      let s = if need_parens r r1 then "(" ^ repr f r1 ^ ")" else repr f r1 in
      s ^ "*"
  | Eps -> "ε"
  | Empty -> "∅"

let char (c : char) : char reg = 
  Lit c

let string (s : string) : char reg = 
  let rec aux i =
    if i >= String.length s then Eps
    else Concat (Lit s.[i], aux (i+1))
  in
  aux 0

let alts (s : string) : char reg =
  let rec aux i = 
    if i >= String.length s then Empty
    else Or (Lit s.[i], aux (i+1))
  in
  aux 0

let accepts (r : char reg) (s : string) : bool = 
  let w = List.init (String.length s) (fun i -> s.[i]) in
  accept r w

let to_string (r : char reg) : string =
  repr (fun c -> Printf.sprintf "%c" c) r

let parse (s : string) : char reg =
  let lexbuf = Lexing.from_string s in
  try
    Parser.regex Lexer.token lexbuf
  with
  | _ -> failwith ("Error: " ^ s)