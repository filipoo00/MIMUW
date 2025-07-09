module Focus where

import Data.Maybe (fromJust)

import Types

goUp :: Focus -> Maybe (Focus)
goUp (Focus _ (TopCtx)) = Nothing
goUp (Focus expr (AppLeft ctx expr2)) = Just (Focus (expr :$ expr2) ctx)
goUp (Focus expr (AppRight expr1 ctx)) = Just (Focus (expr1 :$ expr) ctx)

goLeft :: Focus -> Maybe (Focus)
goLeft (Focus (e1 :$ e2) ctx) = Just (Focus e1 (AppLeft ctx e2))
goLeft _ = Nothing

goRight :: Focus -> Maybe (Focus)
goRight (Focus (e1 :$ e2) ctx) = Just (Focus e2 (AppRight e1 ctx))
goRight _ = Nothing

goToTheNextArg :: Focus -> Maybe (Focus)
goToTheNextArg (Focus expr (AppLeft ctx expr2)) =
  Just (Focus expr2 (AppRight expr ctx))
goToTheNextArg f@(Focus _ (AppRight _ _)) = 
  goUp f >>= goToTheNextArg
goToTheNextArg (Focus _ TopCtx) = Nothing

goToTheNextArgUnsafe :: Focus -> Focus
goToTheNextArgUnsafe = fromJust . goToTheNextArg

goToUpperArg :: Focus -> Maybe (Focus)
goToUpperArg (Focus expr (AppLeft ctx expr2)) =
  Just (Focus (expr :$ expr2) ctx)
goToUpperArg _ = Nothing

changeExpr :: Expr -> Focus -> Focus
changeExpr expr (Focus _ ctx) = Focus expr ctx 

goUpNSteps :: Focus -> Int -> Maybe (Focus)
goUpNSteps focus 0 = Just focus
goUpNSteps focus n = do
  next <- goUp focus
  goUpNSteps next (n - 1)

goUpNStepsUnsafe :: Focus -> Int -> Focus
goUpNStepsUnsafe f n = fromJust (goUpNSteps f n)

leftMostFocusSteps :: Focus -> (Focus, Int)
leftMostFocusSteps f = helper f 0
  where
    helper focus n = case goLeft focus of
      Just f' -> helper f' (n+1)
      Nothing -> (focus, n)