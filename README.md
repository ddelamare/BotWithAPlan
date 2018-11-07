# BotWithAPlan
A Goal Oriented Bot for Starcraft II

Works by the following process
1. Loads available goals and actions into categories (Econ, Army, Upgrades, etc.)
2. On every step calculates a score for each goal in each category and chooses the highest score in that category
3. Dependecies that are declared in the goals are checked to make sure all prerequites are met
4. If all dependecies are met, then uses an action to meet the goal. If not, then and action that fulfils a depenedency is used.
5. Specific unit micro logic is fired every frame independant of the goals chosen


TODOs:
- Better Worker rush defense
- Better Proxy defense
- Other rush denfense
- Drop play
- Proxy play
- Finish 3/3/3 upgrades
- Add battle manager with unit clustering and better defending
- Prioritize pylons supporting lots of buildings
- Improve backline unit micro
- Add harrass unit groups



PRs are welcome!
