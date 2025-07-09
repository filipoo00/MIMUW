open Regextkit
open Tree
open Re

type t = re

let re (s : string) : t = 
  let transform s =
    String.map (function
      | '|' -> '+'
      | c  -> c
    ) s
  in
  
  let s' = transform s in
  parse s'

let debug (r : t) : unit =
  print r

let matches (r : t) (s : string) : bool =
  let w = List.init (String.length s) (fun i -> String.make 1 s.[i]) in

  let rec ders re w =
    match w with
    | [] -> re
    | a :: w' -> ders (simplify (derivative re a)) w'
  in

  is_nullable (ders r w)
