/*
 * Copyright (c) 2019 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/*
 * This program generates Compound/Recurve/Barebow teams based on the
 * qualification scores from the individual qualification scores for
 * the candidate team members. The program tries to put together teams
 * that are reasonably well matched for the day, so that each team has
 * a reasonable chance of winning.
 *
 * The input is three files, each listing archers and their
 * qualification scores. There is a file of compound archers, a file
 * of recurve archers, and a file of barebow archers. The program
 * generates random candidate team mappings, and scores each candidate
 * team mapping. The best team mapping is that which has the lowest
 * score.
 */

# include  <iostream>
# include  <map>
# include  <string>
# include  <vector>
# include  <cstdio>
# include  <cstdlib>
# include  <cassert>

using namespace std;

map<string,unsigned> style_compound;
map<string,unsigned> style_recurve;
map<string,unsigned> style_barebow;

struct team_selection {
      std::string compound_archer;
      unsigned    compound_score;

      std::string recurve_archer;
      unsigned    recurve_score;

      std::string barebow_archer;
      unsigned    barebow_score;
};


static void load_style_list (std::map<std::string,unsigned>&table, FILE*fd)
{
      char line_buffer[1024];

      while (fgets(line_buffer, sizeof line_buffer, fd)) {
	    char name_buffer[512];
	    unsigned score;
	    int rc = sscanf(line_buffer, "%[^,], %u", name_buffer, &score);
	    assert(rc == 2);

	      // Trim leading and trailing white space
	    string use_name = name_buffer;

	    size_t ptr = use_name.find_first_not_of(" \t\r\n");
	    use_name.erase(0, ptr);

	    ptr = use_name.find_last_not_of(" \t\r\n");
	    if (ptr != string::npos) use_name.erase(ptr+1);

	    if (score == 0) {
		  cerr << "Skip athlete: " << use_name << endl;
		  continue;
	    }

	    table[use_name] = score;
      }
}

/*
 * Generate a random team mapping. Scan each archers list (compound,
 * recurve, and barebow) and place the archer into a random team. The
 * result is a fully shuffled team mapping.
 */
static void random_team_mapping(std::vector<struct team_selection>&result)
{
	// Make sure the result table is clear.
      for (size_t ptr = 0 ; ptr < result.size() ; ptr += 1) {
	    result[ptr].compound_score = 0;
	    result[ptr].recurve_score = 0;
	    result[ptr].barebow_score = 0;
      }

      map<string,unsigned>::const_iterator idx;

	// Spread the compound archers randomly into the results table.
      assert(result.size() == style_compound.size());
      for (idx = style_compound.begin() ; idx != style_compound.end() ; ++idx) {
	    assert(idx->second != 0);
	    size_t ptr = random() % result.size();
	    while (result[ptr].compound_score != 0) {
		  ptr = random() % result.size();
	    }

	    result[ptr].compound_archer = idx->first;
	    result[ptr].compound_score  = idx->second;
      }

	// Spread the recurve archers randomly into the results table.
      assert(result.size() == style_recurve.size());
      for (idx = style_recurve.begin() ; idx != style_recurve.end() ; ++idx) {
	    assert(idx->second != 0);
	    size_t ptr = random() % result.size();
	    while (result[ptr].recurve_score != 0) {
		  ptr = random() % result.size();
	    }

	    result[ptr].recurve_archer = idx->first;
	    result[ptr].recurve_score  = idx->second;
      }

	// Spread the barebow archers randomly into the results table.
      assert(result.size() == style_recurve.size());
      for (idx = style_barebow.begin() ; idx != style_barebow.end() ; ++idx) {
	    assert(idx->second != 0);
	    size_t ptr = random() % result.size();
	    while (result[ptr].barebow_score != 0) {
		  ptr = random() % result.size();
	    }

	    result[ptr].barebow_archer = idx->first;
	    result[ptr].barebow_score  = idx->second;
      }
}

static inline unsigned team_score_total(const struct team_selection&team)
{
      return team.compound_score + team.recurve_score + team.barebow_score;
}

/*
 * Score the team mapping. We judge a team mapping by the score, with
 * lower scores better then higher scores. The lowest possible score
 * is zero.
 */
int score_team_mapping(const std::vector<struct team_selection>&teams)
{
      unsigned max_score = team_score_total(teams[0]);
      unsigned min_score = max_score;

      for (size_t idx = 1 ; idx < teams.size() ; idx += 1) {
	    unsigned use_score = team_score_total(teams[idx]);
	    if (use_score > max_score) max_score = use_score;
	    if (use_score < min_score) min_score = use_score;
      }

      return max_score - min_score;
}

int main(int argc, char*argv[])
{
      FILE*fd;

	// Read in the list of compound archers.
      fd = fopen("compound_archers.txt", "rt");
      assert(fd);
      load_style_list(style_compound, fd);
      fclose(fd);

	// Read in the list of recurve archers.
      fd = fopen("recurve_archers.txt", "rt");
      assert(fd);
      load_style_list(style_recurve, fd);
      fclose(fd);

	// Read in the list of barbow archers.
      fd = fopen("barebow_archers.txt", "rt");
      assert(fd);
      load_style_list(style_barebow, fd);
      fclose(fd);

	// At this point, the archers lists should all be the same size.
      assert(style_compound.size() == style_barebow.size());
      assert(style_compound.size() == style_recurve.size());

	// Generate an initial candidate team mapping.
      vector<struct team_selection> candidate_teams;
      candidate_teams.resize(style_compound.size());
      random_team_mapping(candidate_teams);

      int candidate_score = score_team_mapping(candidate_teams);

	// Now generate alternative team mappings, looking for any
	// mapping that is better then the mapping that we have. If we
	// can't find a better guess after a while, then we are done.
      const int RESET_TIMEOUT = 2000000;
      int timeout = RESET_TIMEOUT;
      while (timeout > 0) {
	    vector<struct team_selection> use_teams;
	    use_teams.resize(candidate_teams.size());
	    random_team_mapping(use_teams);

	    int use_score = score_team_mapping(use_teams);
	    if (use_score < candidate_score) {
		  cout << "Found better team mapping (score=" << use_score << ")." << endl;
		  candidate_teams = use_teams;
		  candidate_score = use_score;
		  timeout = RESET_TIMEOUT;
	    } else {
		  timeout -= 1;
	    }
      }

      fd = fopen("generated_teams.txt", "wt");
      for (size_t idx = 0 ; idx < candidate_teams.size() ; idx += 1) {
	    fprintf(fd, "%016s (C), %016s (R), %016s (B):  Total qualifier = %u\n",
		    candidate_teams[idx].compound_archer.c_str(),
		    candidate_teams[idx].recurve_archer.c_str(),
		    candidate_teams[idx].barebow_archer.c_str(),
		    team_score_total(candidate_teams[idx]));
      }

}
