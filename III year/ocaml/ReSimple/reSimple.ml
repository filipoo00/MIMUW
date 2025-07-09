open SimpleRegexp

type t = char reg

let re (s : string) : t = 
  parse s

let debug (r : t) : unit =
  print_endline (to_string r)

let matches (r : t) (s : string) : bool =
  accepts r s