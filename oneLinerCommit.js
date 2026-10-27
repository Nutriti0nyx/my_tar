const { exec } = require("child_process");

exec('rm -f .git/index.lock', () => {
  exec('git add .', () => {
  exec('git commit -m "commit"', (error, stdout, stderr) => {
  console.log(stdout);
   exec('git reset -- oneLinerCommit.js', () => {
   console.log("Commit réussi");
});
});
});
});





