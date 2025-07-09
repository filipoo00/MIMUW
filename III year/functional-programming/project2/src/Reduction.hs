module Reduction where

import qualified Data.Map as Map

import Types

mainExpr :: DefMap -> Maybe Expr
mainExpr defMap = case Map.lookup "main" defMap of
  Just d@(Def _ params expr) -> case params of
    [] -> Just expr
    _ -> error $ "Main function with parameters: " ++ show d
  Nothing -> Nothing

rstep :: DefMap -> Expr -> Maybe Expr
rstep defMap expr =
  let 
    (leftVar, args) = decomposeExpr expr
    numberParamsLeftVar = countParams defMap leftVar
    numberArgsExpr = Just $ length args
  in

  if numberParamsLeftVar == numberArgsExpr then 
    case Map.lookup leftVar defMap of
      Just (Def _ params body) -> 
        let 
          paramsMap = zip params args
          newBody = substAll paramsMap body
        in
        Just newBody 

      Nothing -> Nothing 
  else case expr of 
    e1 :$ e2 -> case rstep defMap e1 of
      Just e1' -> Just (e1' :$ e2)
      Nothing -> case rstep defMap e2 of
        Just e2' -> Just (e1 :$ e2')
        Nothing -> Nothing
    Var name -> case Map.lookup name defMap of
      Just (Def _ [] body) -> Just body
      _ -> Nothing
        
decomposeExpr :: Expr -> (String, [Expr])
decomposeExpr expr = decomposeExprHelper expr []
  where
    decomposeExprHelper :: Expr -> [Expr] -> (String, [Expr])
    decomposeExprHelper (e1 :$ e2) acc = decomposeExprHelper e1 (e2 : acc)
    decomposeExprHelper (Var name) acc = (name, acc)

countParams :: DefMap -> String -> Maybe Int
countParams defMap name = case Map.lookup name defMap of
  Just (Def _ params _) -> Just $ length params
  Nothing -> Nothing 

substAll :: [(Name, Expr)] -> Expr -> Expr
substAll paramsMapList = substAllHelper
  where
    paramsMap = Map.fromList paramsMapList

    substAllHelper (Var name) =
      case Map.lookup name paramsMap of
        Just newExpr -> newExpr
        Nothing -> Var name
    substAllHelper (e1 :$ e2) = substAllHelper e1 :$ substAllHelper e2

rpath :: DefMap -> Expr -> [Expr]
rpath defMap expr = case rstep defMap expr of
  Just nextEpxr -> expr : rpath defMap nextEpxr
  Nothing -> [expr]