module.exports = (grunt) ->
  'use strict'

  grunt.initConfig
    coffee:
      debug:
        files: [
          expand: true
          cwd: 'assets/js'
          src: ['**/*.coffee', '!**/_*.coffee']
          dest: 'public/js'
          ext: '.js'
        ]

    stylus:
      debug:
        files: [
          expand: true
          cwd: 'assets/css'
          src: ['**/*.styl', '!**/_*.styl']
          dest: 'public/css'
          ext: '.css'
        ]

    watch:
      coffee:
        files: ['assets/js/**/*.coffee']
        tasks: ['coffee']

      stylus:
        files: ['assets/css/**/*.styl']
        tasks: ['stylus']

  grunt.loadNpmTasks 'grunt-contrib-coffee'
  grunt.loadNpmTasks 'grunt-contrib-stylus'
  grunt.loadNpmTasks 'grunt-contrib-watch'

  grunt.registerTask 'debug', ['coffee:debug', 'stylus:debug']
  grunt.registerTask 'default', ['debug', 'watch']
