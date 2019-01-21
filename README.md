# team_builder

This is a program to calculate balanced mixed teams for archery
tournaments. The input is a set of qualification scores for compound,
recurve, and barebow archers, and the output is approximately balanced
mixed teams.

The input files have the simple format of one archer and score per
line. For example:

```
   Joe Smith, 290
   Sarah Jones, 291
```

There are three input text files: compound_archers.txt,
recurve_archer.txt, and barebow_archer.txt, that are formatted as
above and contain the archers and scores for each style. The program
assumes that each file has the same number of records.

The output file is called generated_teams.txt and is a human-readable
text file that shows all the calculated teams. Each team has a total
qualifier value which is the sum of the qualifier scores for all the
team members. The program tries to generate teams with total
qualifiers as close together as possible. This should lead to roughly
equally matched teams. The spread of the total qualifiers in the
output give a sense of how well the program did.
