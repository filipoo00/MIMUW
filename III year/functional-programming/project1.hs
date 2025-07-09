infixl 9 :$

data Expr = S | K | I | B
          | Expr :$ Expr
          | X | Z | V Int
          deriving (Show, Read)


test1 = S :$ K :$ K :$ X
twoB = S :$B :$ I
threeB = S :$ B :$ (S :$B :$ I)
test3 = threeB :$ X :$ Z
omega = ((S :$ I) :$ I) :$ ((S :$ I) :$ I)
kio = K :$ I :$ omega
add = (B :$ S) :$ (B :$ B)


prettyExpr :: Expr -> String
prettyExpr e = concat (prettyExprH e [])
  where
    prettyExprH :: Expr -> [String] -> [String]
    prettyExprH S acc = "S" : acc
    prettyExprH K acc = "K" : acc
    prettyExprH I acc = "I" : acc
    prettyExprH B acc = "B" : acc
    prettyExprH X acc = "x" : acc
    prettyExprH Z acc = "z" : acc
    prettyExprH (V a) acc = ("v" ++ show a) : acc
    prettyExprH (e1 :$ e2) acc = prettyExprH e1 (" " : secondExprH e2 acc)

    secondExprH :: Expr -> [String] -> [String]
    secondExprH e@(_ :$ _) acc = "(" : prettyExprH e (")" : acc)
    secondExprH e acc = prettyExprH e acc


rstep :: Expr -> Maybe Expr
rstep (S :$ e1 :$ e2 :$ e3) = Just ((e1 :$ e3) :$ (e2 :$ e3))
rstep (K :$ e1 :$ e2) = Just e1
rstep (I :$ e) = Just e
rstep (B :$ e1 :$ e2 :$ e3) = Just (e1 :$ (e2 :$ e3))
rstep (e1 :$ e2) = case rstep e1 of
  Just e1' -> Just (e1' :$ e2)
  Nothing -> case rstep e2 of
    Just e2' -> Just (e1 :$ e2')
    Nothing -> Nothing
rstep e = Nothing


rpath :: Expr -> [Expr]
rpath expr = 
    let next = rstep expr in 
    case next of
        Nothing -> [expr]
        Just nextExpr -> expr : rpath nextExpr


printPath :: Expr -> IO ()
printPath expr = printPathH (rpath expr) 30
    where
        printPathH :: [Expr] -> Int -> IO ()
        printPathH exprs n = putStrLn (unlines [prettyExpr e | e <- take n exprs])