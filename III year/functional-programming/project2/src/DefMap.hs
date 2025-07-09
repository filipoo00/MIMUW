module DefMap where

import qualified Data.Map as Map
import qualified Data.Set as Set

import Types

checkDuplicateDefs :: Prog -> Maybe [Name]
checkDuplicateDefs (Prog defs) =
  let 
    names = map (\(Def name _ _) -> name) defs
    namesSet = Set.fromList names
  in
  if length names /= Set.size namesSet then
    Just names
  else
    Nothing

buildDefMap :: Prog -> DefMap
buildDefMap (Prog defs) = Map.fromList [(name, def) | def@(Def name _ _) <- defs]