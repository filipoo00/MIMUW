{
open Parser
}

let white = [' ' '\t' '\n']+
let char = ['a'-'z' 'A'-'Z' '0'-'9']

rule token = 
    parse
          white                   { token lexbuf }
        | "|"                     { UNION }
        | "*"                     { STAR }
        | "("                     { LPAR }
        | ")"                     { RPAR }
        | "∅"                     { EMPTY }
        | "ε"                     { EPSILON }
        | char                    { CHAR (Lexing.lexeme lexbuf).[0] }
        | eof                     { EOF }
        | _                       { BADTOK }