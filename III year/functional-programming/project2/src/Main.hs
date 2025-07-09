module Main where

import System.Environment (getArgs)
import Parser (fromHsString)
import Reduction
import DefMap

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
  case checkDuplicateDefs prog of
    Just dups -> error $ "Duplicate definitions: " ++ unwords dups
    Nothing -> do
      let defMap = buildDefMap prog
      case mainExpr defMap of
        Just expr -> do
          putStrLn $ unlines $ take 30 [show x | x <- rpath defMap expr]
        Nothing -> error "No main definition."

usage :: IO ()
usage = do
  putStrLn "Usage: program [--help] [file]"
  putStrLn "  --help  - display this message"
  putStrLn "  file    - input file"