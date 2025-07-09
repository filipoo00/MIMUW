module Types where

import qualified Data.Map as Map

data Def = Def Name [Pat] Expr 
data Expr = Var Name | Expr :$ Expr 
type Pat = Name
type Name = String

newtype Prog = Prog { progDefs :: [Def] }

type DefMap = Map.Map Name Def

instance Show Expr where
  showsPrec _ (Var name) = showString name
  showsPrec p (e1 :$ e2) =
    showParen (p > 10) (showsPrec 10 e1 . showString " " . showsPrec 11 e2)


instance Show Def where
  showsPrec _ (Def name params expr) =
    showString name . showParams params . showString " = " . shows expr
      where
        showParams [] = id
        showParams (x:xs) = showChar ' ' . showString x . showParams xs