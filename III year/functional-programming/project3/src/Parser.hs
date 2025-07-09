module Parser where

import Language.Haskell.Syntax
import Language.Haskell.Parser

import Types

fromHsString :: String -> Prog
fromHsString src = Prog (fromParseResult (parseModule src))

fromParseResult :: ParseResult HsModule -> [Def]
fromParseResult (ParseOk hsModule) = fromHsModule hsModule
fromParseResult (ParseFailed _ err) = error $ "Parse failed: " ++ err

fromHsModule :: HsModule -> [Def]
fromHsModule (HsModule _ _ _ _ decls) =
  let matches = concatMap fromDecl decls
  in [Def [m] | m <- matches]

fromDecl :: HsDecl -> [Match]
fromDecl (HsFunBind matches) = map fromMatch matches

fromDecl (HsPatBind _ pat rhs _) = 
  case fromPatApp pat of
    Just (name, args) -> [Match name args (fromRhs rhs)]
    Nothing -> case pat of
      HsPVar (HsIdent name) -> [Match name [] (fromRhs rhs)]
      _ -> error $ "Illegal HsPatBind: " ++ show pat

fromDecl illegalDecl = error $ "Illegal HsDecl: " ++ show illegalDecl

fromPatApp :: HsPat -> Maybe (Name, [Pat])
fromPatApp (HsPApp (UnQual (HsIdent name)) args) = Just (name, map fromPat args)
fromPatApp (HsPParen p) = fromPatApp p
fromPatApp _ = Nothing

fromMatch :: HsMatch -> Match
fromMatch (HsMatch _ (HsIdent name) pats rhs _) =
  Match name (map fromPat pats) (fromRhs rhs)

fromMatch illegalMatch = error $ "Illegal HsMatch: " ++ show illegalMatch

fromPat :: HsPat -> Pat
fromPat (HsPVar (HsIdent name)) = PVar name
fromPat (HsPApp (UnQual (HsIdent fun)) args) = PApp fun (map fromPat args)
fromPat (HsPParen pat) = fromPat pat
fromPat illegalPat = error $ "Illegal HsPat: " ++ show illegalPat

fromRhs :: HsRhs -> Expr
fromRhs (HsUnGuardedRhs expr) = fromExpr expr
fromRhs illegalRhs = error $ "Illegal HsRhs: " ++ show illegalRhs

fromExpr :: HsExp -> Expr
fromExpr (HsApp fun arg) = fromExpr fun :$ fromExpr arg
fromExpr (HsVar (UnQual (HsIdent name))) = Var name
fromExpr (HsCon (UnQual (HsIdent name))) = Con name
fromExpr (HsLit lit) = Var (literalToString lit)
fromExpr (HsParen expr) = fromExpr expr 
fromExpr illegalExpr = error $ "Illegal HsExpr: " ++ show illegalExpr

literalToString :: HsLiteral -> String
literalToString (HsInt i) = show i
literalToString (HsChar c) = [c]
literalToString (HsString s) = s
literalToString (HsFrac r) = show r
literalToString illegalLit = error $ "Illegal HsLiteral: " ++ show illegalLit