module Main where

import System.Environment (getArgs)
import qualified Data.Map as Map
import Control.Monad.Reader
import Control.Monad.State

import SnocList
import Types
import Parser
import ReductionZipper

buildDefMap :: Prog -> DefMap
buildDefMap (Prog defs) =
  Map.fromListWith combineDefs [ (getName def, def) | def <- defs ]
  where
    getName (Def (m:_)) = matchName m
    getName (Def []) = error "Empty Def"
    
    combineDefs (Def ms1) (Def ms2) = Def (ms2 ++ ms1)

initEvalState :: Expr -> Int -> EvalState
initEvalState expr maxFuel =
  let focus = Focus expr TopCtx
  in EvalState
      { matchEnv = Map.empty
      , fuel = maxFuel
      , currentFocus = focus
      , history = SnocList [focus]
      }

runEval :: DefMap -> Eval a -> (a, EvalState)
runEval env evalAct = 
  runState (runReaderT evalAct env) (initEvalState (Var "main") 30)

main :: IO ()
main = do
  args <- getArgs
  case args of
    ["--help"] -> usage

    [filename] -> do
      contents <- readFile filename
      process contents

    [] -> do
      contents <- getContents
      process contents

    _ -> usage

process :: String -> IO ()
process contents = do
  let prog = fromHsString contents
  let defMap = buildDefMap prog

  let (_, finalState) = runEval defMap (rpath)
  
  let historyList = toList (history finalState)
  printHistory historyList

printHistory :: [Focus] -> IO ()
printHistory [] = return ()
printHistory (x:xs) = do
  print x
  printHistory xs

usage :: IO ()
usage = do
  putStrLn "Usage: program [--help] [file]"
  putStrLn "  --help  - display this message"
  putStrLn "  file    - input file"