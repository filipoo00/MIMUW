module Parser where

import Language.Haskell.Syntax
import Language.Haskell.Parser
import qualified Data.Set as Set

import Types

fromHsString :: String -> Prog
fromHsString src = Prog (fromParseResult (parseModule src))

fromParseResult :: ParseResult HsModule -> [Def]
fromParseResult (ParseOk hsModule) = fromHsModule hsModule
fromParseResult (ParseFailed _ err) = error $ "Parse failed: " ++ err

fromHsModule :: HsModule -> [Def]
fromHsModule (HsModule _ _ _ _ decls) = concatMap fromDecl decls

fromDecl :: HsDecl -> [Def]
fromDecl (HsFunBind matches) = map fromMatch matches
fromDecl (HsPatBind _ (HsPVar (HsIdent name)) rhs _) = 
  [Def name [] (fromRhs rhs)]
fromDecl illegalDecl = error $ "Illegal HsDecl: " ++ show illegalDecl

fromMatch :: HsMatch -> Def
fromMatch (HsMatch _ (HsIdent name) params rhs _) =
  let 
    paramNames = map fromParam params
    paramSet = Set.fromList paramNames
  in
  if length paramNames /= Set.size paramSet then
    error $ "Duplicate parameters: " ++ show paramNames
  else
    Def name paramNames (fromRhs rhs)
fromMatch illegalMatch = error $ "Illegal HsMatch: " ++ show illegalMatch

fromParam :: HsPat -> Pat
fromParam (HsPVar (HsIdent name)) = name
fromParam illegalPat = error $ "Illegal HsPat: " ++ show illegalPat

fromRhs :: HsRhs -> Expr
fromRhs (HsUnGuardedRhs expr) = fromExpr expr
fromRhs illegalRhs = error $ "Illegal HsRhs: " ++ show illegalRhs

fromExpr :: HsExp -> Expr
fromExpr (HsApp fun arg) = fromExpr fun :$ fromExpr arg
fromExpr (HsVar (UnQual (HsIdent name))) = Var name
fromExpr (HsCon (UnQual (HsIdent name))) = Var name
fromExpr (HsParen expr) = fromExpr expr 
fromExpr illegalExpr = error $ "Illegal HsExpr: " ++ show illegalExpr