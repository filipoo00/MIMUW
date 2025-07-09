module ReductionZipper where

import qualified Data.Map as Map
import Control.Monad.Reader
import Control.Monad.State

import Types
import SnocList
import Focus

rstepFocus :: Focus -> Eval (Maybe Focus)
rstepFocus focus = do
  let (leftFocus@(Focus expr _), steps) = leftMostFocusSteps focus
  let (leftExprName, leftExpr, _) = decomposeExpr expr
  numberPatsLeftExpr <- countParams leftExprName

  defMap <- ask
  case numberPatsLeftExpr of
    Just nPats | nPats <= steps -> 
     case Map.lookup leftExprName defMap of
        Just (Def matches) ->
          if nPats == 0 
            then reduceWith matches leftFocus leftFocus nPats
            else do
              let nextFocus = goToTheNextArgUnsafe leftFocus
              reduceWith matches nextFocus leftFocus nPats
        Nothing -> reduceArgsFocus leftFocus
    _ -> case Map.lookup leftExprName defMap of
      Just _ -> 
        case leftExpr of
          Con _ -> reduceArgsFocus leftFocus
          _ -> reduceUpperFocus focus leftFocus
      _ -> reduceArgsFocus leftFocus

  where
    reduceWith :: [Match] -> Focus -> Focus -> Int -> Eval (Maybe Focus)
    reduceWith matches f leftF n = do
      res <- reduceUntilMatchFocus matches f
      case res of 
        Just e' -> do
          let topExpr = goUpNStepsUnsafe leftF n
          let topExpr' = changeExpr e' topExpr
          addFocus topExpr'
          return (Just topExpr')
        Nothing -> reduceArgsFocus leftF
    
reduceUpperFocus :: Focus -> Focus -> Eval (Maybe Focus)
reduceUpperFocus focus leftFocus =
  case goToUpperArg focus of
    Just upperFocus -> do
      addFocus upperFocus
      rstepFocus upperFocus
    Nothing -> reduceArgsFocus leftFocus
    
reduceArgsFocus :: Focus -> Eval (Maybe Focus)
reduceArgsFocus focus = 
  case goToTheNextArg focus of
    Just nextFocus -> do
      addFocus nextFocus
      rstepFocus nextFocus
    Nothing -> return Nothing
  
reduceUntilMatchFocus :: [Match] -> Focus -> Eval (Maybe Expr)
reduceUntilMatchFocus [] _ = return Nothing
reduceUntilMatchFocus (Match _ pats body : ms) focus = do
  oldState <- get
  subEnv <- matchWithReductionFocus pats focus
  case subEnv of
    Just env -> return (Just (subst env body))
    Nothing -> do
      put oldState
      reduceUntilMatchFocus ms focus

matchWithReductionFocus :: [Pat] -> Focus -> Eval (Maybe MatchEnv)
matchWithReductionFocus [] _ = return (Just Map.empty)
matchWithReductionFocus (p:ps) focus@(Focus expr _)
  | notMatchable p expr = return Nothing
  | otherwise =
    case runState (match p expr) Map.empty of
      (True, env1) -> do
        case ps of
          [] -> return (Just env1)
          _ -> do
            let nextFocus = goToTheNextArg focus
            case nextFocus of
              Just nextFocus' -> do
                envRest <- matchWithReductionFocus ps nextFocus'
                case envRest of
                  Just env2 -> return (Just (Map.union env1 env2))
                  Nothing -> return Nothing
              _ -> return Nothing
      (False, _) -> do
        rFocus <- rstepFocus focus
        case rFocus of
          Just rFocus' -> matchWithReductionFocus (p:ps) rFocus'
          Nothing -> return Nothing

notMatchable :: Pat -> Expr -> Bool
notMatchable (PApp name pats) expr =
  let (_, leftExpr, args) = decomposeExpr expr
  in case leftExpr of
      Con leftExprName ->
        leftExprName /= name || length args /= length pats
          || or (zipWith notMatchable pats args)
      _ -> False
notMatchable _ _ = False

match :: Pat -> Expr -> MatchM Bool
match (PVar x) expr = do
  env <- get
  put (Map.insert x expr env)
  return True

match (PApp name pats) expr = do
  let (_, leftExpr, args) = decomposeExpr expr
  case leftExpr of
    Var leftExprName | leftExprName == name && length args == length pats ->
      and <$> zipWithM match pats args
    Con leftExprName | leftExprName == name && length args == length pats ->
      and <$> zipWithM match pats args
    _ -> return False

decomposeExpr :: Expr -> (String, Expr, [Expr])
decomposeExpr expr = decomposeExprHelper expr []
  where
    decomposeExprHelper :: Expr -> [Expr] -> (String, Expr, [Expr])
    decomposeExprHelper (e1 :$ e2) acc = decomposeExprHelper e1 (e2 : acc)
    decomposeExprHelper e@(Var name) acc = (name, e, acc)
    decomposeExprHelper e@(Con name) acc = (name, e, acc)

countParams :: String -> Eval (Maybe Int)
countParams name = do
  defMap <- ask
  case Map.lookup name defMap of
    Just (Def (Match _ params _ : _)) -> return (Just (length params))
    _ -> return Nothing

subst :: MatchEnv -> Expr -> Expr
subst env = substHelper
  where
    substHelper (Var name) =
      case Map.lookup name env of
        Just newExpr -> newExpr
        Nothing -> Var name

    substHelper (Con name) = Con name
    substHelper (e1 :$ e2) = substHelper e1 :$ substHelper e2

rpath :: Eval ()
rpath = do
  st <- get
  if fuel st <= 0
    then return ()
    else do
      nextFocus <- rstepFocus (currentFocus st)
      case nextFocus of
        Just _ -> do
          modify (\s -> s { fuel = fuel s - 1 })
          rpath
        Nothing -> return ()

addFocus :: Focus -> Eval ()
addFocus focus = do
  st <- get
  let newHistory = snoc (history st) focus
  modify $ \s -> s { currentFocus = focus, history = newHistory }