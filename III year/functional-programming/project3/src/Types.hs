module Types where

import qualified Data.Map as Map
import Control.Monad.Reader
import Control.Monad.State

import SnocList

type Name = String
data Def = Def { defMatches :: [Match] }
data Match = Match
    { matchName :: Name
    , matchPats :: [Pat]
    , matchRhs  :: Expr
    }

infixl 9 :$
data Expr
    = Var Name
    | Con Name
    | Expr :$ Expr
    deriving Eq
data Pat = PVar Name | PApp Name [Pat]

newtype Prog = Prog { progDefs :: [Def] }

type DefMap = Map.Map Name Def

data Context = 
  TopCtx
  | AppLeft Context Expr
  | AppRight Expr Context

data Focus = Focus
  { focusExpr :: Expr
  , focusContext :: Context
  }

type History = SnocList Focus

type MatchEnv = Map.Map Name Expr
type MatchM = State MatchEnv

data EvalState = EvalState 
  { matchEnv :: MatchEnv
  , history :: History
  , fuel :: Int
  , currentFocus :: Focus
  }

type Eval a = ReaderT DefMap (State EvalState) a 

instance Show Expr where
  showsPrec _ (Var name) = showString name
  showsPrec _ (Con name) = showString name
  showsPrec p (e1 :$ e2) =
    showParen (p > 9) (showsPrec 9 e1 . showString " " . showsPrec 10 e2)

instance Show Pat where
  showsPrec _ (PVar name) = showString name
  showsPrec _ (PApp name []) = showString name
  showsPrec _ (PApp name pats) =
    showParen True $ showString name . showPats pats
      where
        showPats [] = id
        showPats (x:xs) = showChar ' ' . shows x . showPats xs

instance Show Match where
  showsPrec _ (Match name pats rhs) =
    showString name . showPats pats . showString " = " . shows rhs
      where
        showPats [] = id
        showPats (x:xs) = showChar ' ' . shows x . showPats xs

instance Show Def where
  showsPrec _ (Def []) = id
  showsPrec p (Def (m:ms)) = 
    showsPrec p m . showString "\n" . showsPrec p (Def ms)

instance Show Focus where
  show (Focus expr c) = rebuild c (showFocused expr) ""
    where
      showFocused e = showChar '{' . shows e . showChar '}'

      rebuild TopCtx exprString = exprString

      rebuild (AppLeft ctx expr2) exprString = case ctx of
        AppRight _ _ -> 
          rebuild ctx $ showChar '(' . exprString . showChar ' ' . addParens expr2 . showChar ')'
        _ -> 
          rebuild ctx $ exprString . showChar ' ' . addParens expr2
        where
          addParens e@(_ :$ _) = showParen True (shows e)
          addParens e = shows e

      rebuild (AppRight expr1 ctx) exprString = case ctx of
        AppRight _ _ -> 
          rebuild ctx $ showChar '(' . shows expr1 . showChar ' ' . exprString . showChar ')'
        _ -> 
          rebuild ctx $ shows expr1 . showChar ' ' . exprString
