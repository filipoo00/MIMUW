%token <char>       CHAR
%token              UNION CONCAT STAR
%token              EMPTY EPSILON
%token              LPAR RPAR
%token              EOF BADTOK

/* associativity and precedence, lowest to highest */
%left               UNION
%left               CONCAT
%left               STAR
%nonassoc           LPAR RPAR
%nonassoc           CHAR EPSILON EMPTY

%type <char SimpleRegexpDef.reg>     regex
%start              regex

%%

regex :
      re EOF                        { $1 } ;

re :
      CHAR                          { Lit $1 }
    | EPSILON                       { Eps }
    | EMPTY                         { Empty }
    | LPAR re RPAR                  { $2 }
    | re UNION re                   { Or ($1, $3) }
    | re CONCAT re                  { Concat ($1, $3) }
    | re re %prec CONCAT            { Concat ($1, $2) }
    | re STAR                       { Star $1 } ;