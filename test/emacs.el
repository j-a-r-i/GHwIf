(defun eshell/ut-old (&rest args)
  (eshell-eval-using-options
   "ut" args
   '((?l "ls"  nil ls   "list unit tests")
     (?x "exe" t   aexe "execute specific test")
     (?t "test" t   test "testing")
     (nil "help" nil nil "usage")
     :show-usage)
   (eshell-do-eval
    (eshell-parse-command
     (cond
      (ls "./utest --gtest_list_tests")
      (test
       (message "argument %s" test)
       "free")
      (aexe
       (message "argument %s" aexe)
       (format "./utest --gtest_filter=%s" aexe))))
    t)))

(defun eshell/ut (cmd)
  (eshell-do-eval
   (eshell-parse-command
    (cond
     ((string= cmd "ls")
      "./utest --gtest_list_tests\n")
     ((string= cmd "help")
      "./utest --help\n")
     (t
      (format "./utest --gtest_filter=%s" cmd))))))
