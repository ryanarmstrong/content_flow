<ul class="nav navbar-nav navbar-right">
  <?php foreach ($items as $item): ?>
    <li <?php if(isset($item['content classes'])) { print $item['content classes']; } ?>><?php print $item['content']; ?></li>
  <?php endforeach; ?>
</ul>
