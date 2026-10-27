const { exec } = require("child_process");

exec('rm -f .git/index.lock', (error, stdout, stderr) => {
  exec('git add .', (error, stdout, stderr) => {
  exec('git commit -m "commit"', (error, stdout, stderr) => {
  console.log(stdout);
   exec('git reset -- oneLinerCommit.js', (error, stdout, stderr) => {
   console.log("Commit réussi");
});
});
});
});





